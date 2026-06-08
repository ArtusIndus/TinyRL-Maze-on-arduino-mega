/*
  
Copyright (c) 2026 ArtusIndus
  TinyRL-Maze (Arduino Mega)
  Fixed Version

  Commands:
    start x y
    goal x y
    status
    policy
    reset
    help
*/

#include <string.h>

const byte W = 15;
const byte H = 15;

const long MAX_EPISODES = 50000;
const int MAX_STEPS = 400;

// Static maze layout
// '#' = wall
// 'S' = default start marker in map design
// 'G' = default goal marker in map design
const char maze[H][W + 1] = {
  "###############",
  "#S..#.....#...#",
  "#.#.#.###.#.#.#",
  "#.#...#...#.#.#",
  "#.#####.#.###.#",
  "#.....#.#.....#",
  "###.#.#.#####.#",
  "#...#.#.....#.#",
  "#.###.#####.###",
  "#...#.....#.#.#",
  "#.#####.#.#.#.#",
  "#.....#.#.#...#",
  "#.###.#.#.###.#",
  "#.....#......G#",
  "###############"
};

const byte ACTIONS = 4;

// Q-table:
// Q[x][y][action]
float Q[W][H][ACTIONS];

// Q-Learning parameters
float alpha = 0.15f;   // learning rate
float gamma = 0.95f;   // discount factor
float epsilon = 0.20f; // exploration rate

bool ready = false;

bool startSet = false;
bool goalSet  = false;

int startX = -1;
int startY = -1;

int goalX = -1;
int goalY = -1;

// Training statistics
long episode = 0;
long successCount = 0;
long totalSteps = 0;

// --------------------------------------------------
// Returns true if position is outside the maze
// or contains a wall tile.
bool isWall(int x, int y)
{
  if (x < 0 || x >= W) return true;
  if (y < 0 || y >= H) return true;

  return maze[y][x] == '#';
}

// --------------------------------------------------
// Checks whether the environment is fully configured
// and ready for training.
void checkReady()
{
  ready = false;

  if (!startSet)
    return;

  if (!goalSet)
    return;

  if (isWall(startX, startY))
    return;

  if (isWall(goalX, goalY))
    return;

  if (startX == goalX &&
      startY == goalY)
    return;

  ready = true;

  Serial.println();
  Serial.println("================================");
  Serial.println("Environment ready.");
  Serial.println("Training started.");
  Serial.println("================================");
}

// --------------------------------------------------
// Clears all learned Q-values and statistics.
void resetLearning()
{
  memset(Q, 0, sizeof(Q));

  episode = 0;
  successCount = 0;
  totalSteps = 0;

  Serial.println("Learning reset.");
}

// --------------------------------------------------
// Epsilon-greedy action selection.
// With probability epsilon choose random action,
// otherwise choose best known action.
int chooseAction(int x, int y)
{
  if (random(1000) < (long)(epsilon * 1000))
    return random(ACTIONS);

  int bestA = 0;
  float bestQ = Q[x][y][0];

  for (int a = 1; a < ACTIONS; a++)
  {
    if (Q[x][y][a] > bestQ)
    {
      bestQ = Q[x][y][a];
      bestA = a;
    }
    else if (Q[x][y][a] == bestQ && random(2))
    {
      // Random tie-breaking
      bestA = a;
    }
  }

  return bestA;
}

// --------------------------------------------------
// Returns the highest future Q-value
// for a given state.
float bestFuture(int x, int y)
{
  float best = Q[x][y][0];

  for (int a = 1; a < ACTIONS; a++)
  {
    if (Q[x][y][a] > best)
      best = Q[x][y][a];
  }

  return best;
}

// --------------------------------------------------
// Calculates next position for an action.
void moveAgent(int x, int y, int a, int &nx, int &ny)
{
  nx = x;
  ny = y;

  switch (a)
  {
    case 0: ny--; break; // up
    case 1: nx++; break; // right
    case 2: ny++; break; // down
    case 3: nx--; break; // left
  }
}

// --------------------------------------------------
// Executes one complete training episode.
// Returns true if the goal was reached.
bool runEpisode(int &stepsUsed)
{
  int x = startX;
  int y = startY;

  for (int step = 0; step < MAX_STEPS; step++)
  {
    int a = chooseAction(x, y);

    int nx, ny;
    moveAgent(x, y, a, nx, ny);

    int reward;

    // Wall collision
    if (isWall(nx, ny))
    {
      reward = -30;
      nx = x;
      ny = y;
    }
    // Goal reached
    else if (nx == goalX && ny == goalY)
    {
      reward = 100;
    }
    // Normal movement penalty
    else
    {
      reward = -1;
    }

    // Standard Q-Learning update
    Q[x][y][a] =
      Q[x][y][a] +
      alpha *
      (
        reward +
        gamma * bestFuture(nx, ny) -
        Q[x][y][a]
      );

    x = nx;
    y = ny;

    if (x == goalX && y == goalY)
    {
      stepsUsed = step + 1;
      return true;
    }
  }

  stepsUsed = MAX_STEPS;
  return false;
}

// --------------------------------------------------
// Executes a fully greedy run using the current policy.
// Returns the number of steps to goal,
// or 0 if goal was not reached.
int greedyRun()
{
  int x = startX;
  int y = startY;

  for (int step = 0; step < MAX_STEPS; step++)
  {
    int bestA = 0;
    float bestQ = Q[x][y][0];

    for (int a = 1; a < ACTIONS; a++)
    {
      if (Q[x][y][a] > bestQ)
      {
        bestQ = Q[x][y][a];
        bestA = a;
      }
    }

    int nx, ny;
    moveAgent(x, y, bestA, nx, ny);

    if (isWall(nx, ny))
    {
      nx = x;
      ny = y;
    }

    x = nx;
    y = ny;

    if (x == goalX && y == goalY)
      return step + 1;
  }

  return 0;
}

// --------------------------------------------------
// Prints the currently learned policy.
// Arrows indicate the preferred action.
void printPolicy()
{
  Serial.println();
  Serial.println("===== POLICY =====");

  for (int y = 0; y < H; y++)
  {
    for (int x = 0; x < W; x++)
    {
      if (maze[y][x] == '#')
      {
        Serial.print('#');
        continue;
      }

      if (x == startX && y == startY)
      {
        Serial.print('S');
        continue;
      }

      if (x == goalX && y == goalY)
      {
        Serial.print('G');
        continue;
      }

      int bestA = 0;
      float bestQ = Q[x][y][0];

      for (int a = 1; a < ACTIONS; a++)
      {
        if (Q[x][y][a] > bestQ)
        {
          bestQ = Q[x][y][a];
          bestA = a;
        }
      }

      char c = '^';

      if (bestA == 1) c = '>';
      if (bestA == 2) c = 'v';
      if (bestA == 3) c = '<';

      Serial.print(c);
    }

    Serial.println();
  }
}

// --------------------------------------------------
// Processes user commands from the serial interface.
void handleSerial()
{
  if (!Serial.available())
    return;

  String cmd = Serial.readStringUntil('\n');
  cmd.trim();

  int x, y;

  if (sscanf(cmd.c_str(), "start %d %d", &x, &y) == 2)
  {
    if (!isWall(x, y) &&
        !(x == goalX && y == goalY))
    {
      startX = x;
      startY = y;

      startSet = true;

      resetLearning();
      checkReady();

      Serial.print("Start set: ");
      Serial.print(startX);
      Serial.print(" ");
      Serial.println(startY);
    }
  }
  else if (sscanf(cmd.c_str(), "goal %d %d", &x, &y) == 2)
  {
    if (!isWall(x, y) &&
        !(x == startX && y == startY))
    {
      goalX = x;
      goalY = y;

      goalSet = true;

      resetLearning();
      checkReady();

      Serial.print("Goal set: ");
      Serial.print(goalX);
      Serial.print(" ");
      Serial.println(goalY);
    }
  }
  else if (cmd == "reset")
  {
    resetLearning();
    checkReady();
  }
  else if (cmd == "policy")
  {
    printPolicy();
  }
  else if (cmd == "status")
  {
    Serial.println();

    Serial.print("Episode: ");
    Serial.println(episode);

    float rate = 0;

    if (episode > 0)
      rate = 100.0f * successCount / episode;

    Serial.print("Success Rate: ");
    Serial.print(rate);
    Serial.println("%");

    int greedy = greedyRun();

    Serial.print("Greedy Steps: ");
    Serial.println(greedy);
  }
  else if (cmd == "help")
  {
    Serial.println();
    Serial.println("start x y");
    Serial.println("goal x y");
    Serial.println("status");
    Serial.println("policy");
    Serial.println("reset");
  }
}

// --------------------------------------------------
// Arduino setup routine.
void setup()
{
  Serial.begin(115200);

  // Seed random generator from analog noise
  randomSeed(analogRead(A0));

  Serial.println();
  Serial.println("================================");
  Serial.println("TinyRL-Maze");
  Serial.println("================================");
  Serial.println("Waiting for configuration...");
  Serial.println();
  Serial.println("Commands:");
  Serial.println("start x y");
  Serial.println("goal x y");
  Serial.println("status");
  Serial.println("policy");
  Serial.println("reset");
  Serial.println("help");
  Serial.println();
  Serial.println("Example:");
  Serial.println("start 1 1");
  Serial.println("goal 13 13");
  Serial.println();
}

// --------------------------------------------------
// Main Arduino loop.
void loop()
{
  handleSerial();

  if (!ready)
{
  static unsigned long lastMsg = 0;

  // Periodically remind the user that
  // start and goal positions are missing.
  if (millis() - lastMsg > 5000)
  {
    lastMsg = millis();

    Serial.println(
      "Waiting for START and GOAL..."
    );
  }

  return;
}

  if (!ready)
    return;

  if (episode >= MAX_EPISODES)
  {
    Serial.println();
    Serial.println("Training complete.");

    printPolicy();

    // Stop execution permanently
    while (true)
    {
    }
  }

  int steps = 0;

  bool solved = runEpisode(steps);

  episode++;

  if (solved)
  {
    successCount++;
    totalSteps += steps;
  }

  // Print progress every 500 episodes
  if (episode % 500 == 0)
  {
    Serial.println("----------------------");

    Serial.print("Episode: ");
    Serial.println(episode);

    float rate = 100.0f * successCount / episode;

    Serial.print("Success Rate: ");
    Serial.print(rate);
    Serial.println("%");

    if (successCount > 0)
    {
      Serial.print("Avg Steps: ");
      Serial.println(
        (float)totalSteps /
        (float)successCount
      );
    }

    Serial.print("Greedy Path: ");
    Serial.println(greedyRun());

    Serial.println("----------------------");
  }
}
