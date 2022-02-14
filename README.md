# Intro-to-Robotics


## Description
In this repository you can find implementations in C++ of Krembots behaviors for ARGoS simulator.

![image](https://user-images.githubusercontent.com/72878018/153824673-beb81f26-1070-4c31-a9aa-152c3d688241.png)
![image](https://user-images.githubusercontent.com/72878018/153824729-970bbb09-58ed-4905-8e87-b6ee3105596c.png)
![image](https://user-images.githubusercontent.com/72878018/153825157-e49bc2fd-eb51-4842-8429-ac8fb14d443b.png)

**ARGoS Simulator** is a multi-physics robot simulator that can simulate large-scale swarms of robots of any kind efficiently. You can find more information about ARGoS Sim on ARGoS website https://www.argos-sim.info/.

**Krembot Simulator** is a physics-based 3D simulator for Robotican's Krembot robot. Original code by Elhay Rauper for Bar Ilan University and the ISF Smart Swarms center. Uses and modifies (and adds to) the Argos multi-robot simulator.

In ARGoS, the software component that simulates a real robot is named Footbot. The FootBot version of Computer Science department in Bar-Ilan University is named Krembot.


## Installation
1. **Prerequisite:** Linux OS. Simulation was tested on Linux Ubuntu 16.04.6 LTS/Mint 18.4.
2. **ARGoS Core:** The core of the ARGoS Simulator. You can download it from https://www.argos-sim.info/core.php and install it.
3. **krembot_sim:** A physics-based 3D simulator for Robotican's Krembot robot. You can download it from https://bitbucket.org/galk-opensource/krembot_sim/src/master/ and install it according to the instructions written in the link.
4. **krembot_ws:** This is the workspace of the krembot. You can download it from this repository, or create one of your own by copy the directory "krembot_ws" from "krembot_sim" to a path as you wish.


## Configurations
A mission or experiment are defined by _.argos_ files that are kind of XMLs that setting up the arena, its size, obstacles in it and other experiment preferences. For each mission/experiment/exercise, there is an _.argos_ file in the directory krembot_ws/config. The _.argos_ file is referencing C++ files that defines Krembots' behaviors.

To run an experiment use the command:
> argos 3 -c <_config_>
 
where <_config_> should be replaced by the path to the _.argos_ file that defines the experiment.


## Controllers
As was said before, Krembots' behaviors are defined by C++ files. A C++ program that defines behavior of a Krembot for a specific task is named "Controller". A controller usually contains a _.cpp_ source file, a _.h/.hpp_ header file, a _CMakeLists.txt_ file, and maybe other supportive files. Each mission/experiment requires a krembot's controller to make the krembots behave in such a way that they will fulfill their task. The controllers are located in "krembot_ws/controllers". Each experiments has its own controller, referenced by its unique _.argos_ file.


## Exercise 1 - Warmup
This exercise is a warm-up exercise only. It is designed to install the working environment and to make sure all the simulator components are working properly. As part of this exercise, we have implemented a simple controller code that causes the kremboot to drive straight and stop about 15 meters from an obstacle in front. You can find the experiment's definition in _krembot_ws/config/ex1.argos_, and the controller implementation in _krembot_ws/controllers/ex1_.

To run the experiment, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh warmup

In addition, it is recommended to make sure that the following controller examples are working properly:
> $ scripts/run.sh mixed_controllers

> $ scripts/run.sh porting_example

> $ scripts/run.sh run_away_by_bumpers

> $ scripts/run.sh run_away_by_bumpers_multiple

> $ scripts/run.sh sand_timer_test

> $ scripts/run.sh simple_demo


## Exercise 2 - Wanderbot

In this exercise we have implemented a coverage algorithm that let the krembot wander around randomly in strait lines until most of the arena is covered.

**Algorithm Description:** The algorithm let the robot drive at full speed until one of its front bumpers pressed, and then stop and change driving angle by turning in a random speed and direction for a random time (up to 3 seconds), and then drive again and so on.

Note that we inferred that it doesn't matter if one of the other bumpers pressed because the driving is forward. We found that the rear bumpers never pressed, but sometimes the right or the left bumpers are pressed. We decided to ignore side presses because the driving is forward so we can save turnings.

For more information about the exercise you can read the full report: https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics%20-%20ex2%20-%20Report.pdf.

To run the experiment, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh wanderbot


## Exercise 3 - Navigation with PRM

In this exercise we have implemented an algorithm that let the Krembot navigate from the starting position (1,1) to the goal position (-2,-2) using Probabilistic Roadmap, or PRM. We will describe the setup and loop functions, the positions sampling method, the distance metric, the use of KNN and KD-Tree data structure, the shortest path algorithm, and the robot's feedback-control. 

### Algorithm Description
The algorithm is divided to the setup algorithm (planner) and the loop function (execution).

**Setup:** The setup algorithm follows the following steps:
1. Let s be the starting position (1,1) and let t be the goal position (-2,-2).
2. Inflate each obstacle in the grid w.r.t the robot size (we set it as constant 0.2).
3. Lower the resolution of the grid such that a cell in the low-resolution grid represents multiple cells in the high-resolution grid, and there is 1 in each low-resolution cell that represents a group of high-resolution cells that one of them contains 1 (and 0 o.w).
4. PRM - Sample 12,000 positions from the arena. The number of samples should be changed w.r.t the size of the arena.
5. Insert each sampled position to a KD-Node.
6. Create KD-Tree out of the KD-Nodes.
7. Calculate adjacent-matrix using KNN which use the KD-Tree for better performance. 
8. Find the shortest path between s and t using Dijkstra and the adjacent-matrix.

**Loop:** the algorithm gets the shortest path from s to t and follows the following steps:
1. If current position is goal – then stop (goal reached).
2. Find the next position.
3. Turn until positioned toward the next position.
4. Drive to the next position.
5. Back to 1.

**Sampling Method:** we used uniform sampling of positions in order to be able to find the shortest path in different arenas without knowing the positions of the obstacles in advance. We tried to sample different numbers of positions and decided that sampling size of 12,000 position is a good practice for an arena at size 250×250.

Examples:
1.	Sample 2 position: the algorithm couldn't find a path from s to t due to many connected components.
2.	Sample 6,000 positions: the algorithm succeeds to find path from s to t, but the robot lingered between adjacent obstacles so we assume that in an arena with a lot of obstacles the robot's performance might be poor.
3.	Sample 12,000 positions: the algorithm succeeds to find path from s to t in all of our tests.

**Distance Metric:** we have implemented two kinds of metrics:
1. L1 Metric – Manhattan Distance.
2. L2 Metric – Euclidean Distance.

Comparison of results:

![image](https://user-images.githubusercontent.com/72878018/153836497-65ed35a0-1428-4169-8c3b-57751446b735.png)
 	 
It seems that better performance is achieved by using the L2 metric, i.e., the robot navigates from (1,1) to (-2,-2) in a shorter path. This makes sense, since the L1 metric calculates the Manhattan Distance while the robot can move at any angle it chooses (i.e., also diagonally, not just up, down, right, and left). Hence, we preferred using L2 distance metric.

**Data Structure:** In order to calculate the adjacent-matrix, we needed to run KNN algorithm and to do so in good performance, we needed a unique data structure that stores information about points in the space (on the grid) in such a way that would fit the KNN algorithm and allow it to run quickly. The data structure that allows this is KD-Tree.

From Wikipedia:

_In computer science, a k-d tree (short for k-dimensional tree) is a space-partitioning data structure for organizing points in a k-dimensional space. k-d trees are a useful data structure for several applications, such as searches involving a multidimensional search key (e.g., range searches and nearest neighbor searches) and creating point clouds. k-d trees are a special case of binary space partitioning trees._

For further reading: https://en.wikipedia.org/wiki/K-d_tree

We used an implementation of KD-Tree from the internet: https://github.com/cdalitz/kdtree-cpp

**KNN Calibration:** We were looking for a good hyper-parameter for KNN, i.e., a number k that let the KNN associate enough neighbors for each position, so that running Dijkstra on the resulting graph will lead to a path from the starting position to the goal. On the one hand, if we use too small k, the algorithm may advance in very small steps or even won't find any path to the goal. On the other hand, if we use too large k, the algorithm may take a very long time to run. So, we have tried different ks and decided that k=30 is a fair choice.

For example:
1. In k=2, the algorithm didn't find a path to the goal.
2. In k=10, the algorithm found a path, but the robot advanced in very small steps.
3. In k=30, the algorithm found a path, and the robot advanced in reasonable steps.

**Local Planner:** Because of the robot is given a task once (navigate from (1,1) to (-2,-2)), its local planner is implemented in the setup function as described above.
The planner works as follows:

define s,t ⟹ inflate obstacles ⟹ lower resolution ⟹ sample positions ⟹ create KDNode for each position ⟹ create KDTree ⟹ Run KNN to get AdjMatrix ⟹ Run Dijkstra to find shortest path from s to t.

To make the robot able to receive new tasks and plan their execution in running time, it will be necessary to separate the part of the local planner from the setup function into a separate code section (suppose a function) and call that section each time a new task is received. In reality, it is better to run this code in a separate thread to avoid the robot from being stop whenever it is planning a new task.

**Shortest-Path Algorithm:** In the query phase, we decided to use Dijkstra algorithm for finding the shortest path from a sampled position very close to the starting position to a sampled position very close to the goal.

**Feedback-Control:** The planner gives the robot a path consists of waypoints (positions). In each waypoint, the robot calculates the angle to the next waypoint and start turning in angular speed of 30 or -30 (depending on which side the rotation time will be shorter), until it is positioned toward the next waypoint with error angle of ±5°. Then, the robot start driving until it reaches a position "close enough" to the next waypoint.

**Speed-Angle Trade-off:** there is a trade-off between the angular speed when the robot is turning, to the error angle. We could deny any error angle, and makes the robot drive only when it is positioned straight towards the next waypoint, but in this way the angular speed should be very slow because otherwise the robot may miss the perfect angle and will have to complete another circle (and then again it is possible to miss the perfect angle). As we do not want a very slow angular speed and neither a wide error angle, we had to find numbers that will work well together. In the end we determined those numbers (hard-coded) as angularSpeed=30 and errorAngle=5°.

**Reaching Next Waypoint:** Now, we still have to explain what is it means "close enough to the next waypoint". When the robot is driving to the next waypoint that represented as a point in space, it is not reasonable that the robot will reach the exact position perfectly due to small angle errors and stopping times. Hence, we had to find a number that means "close enough" to the waypoint, and we determined this number as 2. This is reflected in the code through the function close_enough() that calculates the distance (using L2 metric) and return ((0 <= distance) && (distance <= 2)) as Boolean.

For more information about the exercise you can read the full report: https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics%20-%20ex3%20-%20Report.pdf.

To run the experiment, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh navigation


### Exercise 4 - Foraging Tournament


## IDEs, Writers and Tools

1. JetBrains CLION
2. Visual Studio Code
3. Notepad++
4. Oracle VirtualBox VM
5. Windows Subsystem for Linux 2


## Notes
* For your ease, you can use the scripts in the krembot_ws/scripts directory to compile the project and run a simulation easily.



## Screenshots

