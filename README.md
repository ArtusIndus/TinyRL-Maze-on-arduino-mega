# TinyRL-Maze Q-Learning on Arduino

A minimal reinforcement learning system running directly on an Arduino Mega, solving a grid-based maze using tabular Q-Learning.

No frameworks. No Python. Pure embedded C++.

---

## Overview

TinyRL-Maze demonstrates how an autonomous agent can learn to navigate a maze using reinforcement learning under strict hardware constraints.

The system trains directly on-device and continuously improves its policy through interaction with a fixed environment.

---

## Key Features

- Pure C++ Q-Learning implementation without external libraries  
- Real-time training on Arduino Mega  
- 2D grid-world maze environment  
- Goal-oriented reward system  
- ε-greedy exploration strategy  
- Live training statistics via Serial Monitor  
- Policy visualization using ASCII directional output  

---

## How It Works

The agent learns an optimal policy using the Q-Learning update rule:

Q(s, a) ← Q(s, a) + α [ r + γ max Q(s', a') − Q(s, a) ]

Where:
- α is the learning rate  
- γ is the discount factor  
- ε is the exploration rate  

The agent iteratively improves its pathfinding strategy until convergence.

---

## Environment

- Grid size: 15 × 15  
- Actions: Up, Right, Down, Left  
- Rewards:
  - Goal: +100  
  - Wall collision: -30  
  - Step cost: -1  

---

## Serial Commands

Interaction happens via Serial Monitor:

start x y     → set starting position  
goal x y      → set goal position  
status        → show training statistics  
policy        → print learned policy  
reset         → reset Q-table  
help          → show available commands  

---

## Results

After training, the agent converges to a stable optimal policy:

- High success rate (>99%)  
- Stable shortest-path behavior  
- Deterministic greedy route after convergence  

Example output:
```cpp
Episode: 1500 
Success Rate: 98.60% 
Avg Steps: 36.10 
Greedy Path: 24
```
---

## Why This Project Matters

TinyRL-Maze shows that reinforcement learning can run on microcontrollers.

It demonstrates that:
- complex behavior can emerge from simple update rules  
- embedded systems can perform autonomous learning  
- real-time RL does not require large compute resources  

This project is a foundation for:
- autonomous robotics  
- edge AI systems  
- self-learning embedded agents  

---

## Future Work

- Dynamic maze generation  
- SARSA vs Q-Learning comparison  
- Multi-goal environments  
- Neural network-based Q-function approximation  
- Integration with real robot hardware  

---

## License

MIT License
