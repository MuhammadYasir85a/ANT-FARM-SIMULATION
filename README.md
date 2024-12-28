# ANT-FARM-SIMULATION
#TASK 02 

Description
The Ant Farm Simulation is a C++ program that models the behavior and management of ant colonies. Users can create and control multiple colonies, each with different types of ants (Drones, Warriors, and Queens) and rooms (Spawner and Relax rooms). The simulation operates in ticks, allowing ants to gather resources, rest, and engage in battles. This project demonstrates object-oriented programming concepts and design patterns in C++.

Features
Create and manage ant colonies.
Different ant types with unique behaviors.
Rooms for spawning and resting ants.
Tick-based simulation for dynamic interactions.
How to Use
Clone the repository.
Compile the code using a C++ compiler.
Run the program and enter commands to interact with the simulation
OUTPUT:
AntFarm Simulation Initiated. Enter commands (type 'exit' to terminate):
> spawn
New colony spawned with ID: 1
> give 1 water 100
> give 1 food 100
> give 1 warrior 19
> give 1 drone 3
> summary 1
Colony ID: 1
Species: generic
Ant Count: 23
Food Stock: 100
Water Stock: 100
> tick alive
SpawnerRoom is producing new ants.
RelaxRoom is providing rest to ants.
Simulation ends: One or fewer active colonies remain.
