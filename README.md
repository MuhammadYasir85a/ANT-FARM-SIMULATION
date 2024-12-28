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
> spawn 14 -32 Killer
New colony spawned at (14,-32) with ID: 13968
> give 13968 warrior 10
> give 13968 food 50
> tick 10
> summary 13968
Species: Killer
Workers: 0
Warriors: 10
Ant Kills: 0
Colony kills: 0
Ticks alive: 10
Status: Alive
> give 13968 drone 14
> tick 100
> summary 13968
Species: Killer
Workers: 14
Warriors: 10
Ant Kills: 0
Colony kills: 0
Ticks alive: 110
Status: Alive
> spawn 20 -40 Gatherer
New colony spawned at (20,-40) with ID: 19960
> tick 50
> summary 13968
Species: Killer
Workers: 14
Warriors: 10
Ant Kills: 0
Colony kills: 0
Ticks alive: 160
Status: Alive
> summary 19960
Species: Gatherer
Workers: 0
Warriors: 0
Ant Kills: 0
Colony kills: 0
Ticks alive: 50
Status: Alive
>
