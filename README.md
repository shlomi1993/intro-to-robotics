# Intro-to-Robotics

Introduction to Robotics is a course I took at Bar-Ilan University as part of Computer Science (B.Sc) studies. The course was about theoretical computer science problems in the field of robotics.

## Description
In this repository you can find implementations in C++ of Krembots behaviors for ARGoS simulator.

![image](https://user-images.githubusercontent.com/72878018/153840327-123a6892-6c9a-420d-8570-128641f730ca.png)

(image source: https://www.argos-sim.info/examples.php)

**ARGoS Simulator** is a multi-physics robot simulator that can simulate large-scale swarms of robots of any kind efficiently. You can find more information about ARGoS Sim on ARGoS website https://www.argos-sim.info/.

**Krembot Simulator** is a physics-based 3D simulator for Robotican's Krembot robot. Original code by Elhay Rauper for Bar Ilan University and the ISF Smart Swarms center. Uses and modifies (and adds to) the Argos multi-robot simulator.

**Note:** In ARGoS, the software component that simulates a real robot is named Footbot. The FootBot version of Computer Science department in Bar-Ilan University is named Krembot.


## Installation
1. **Prerequisite:** Linux OS. Simulation was tested on Linux Ubuntu 16.04.6 LTS/Mint 18.4.
2. **ARGoS Core:** The core of the ARGoS Simulator. You can download it from https://www.argos-sim.info/core.php and install it.
3. **krembot_sim:** A physics-based 3D simulator for Robotican's Krembot robot. You can download it from https://bitbucket.org/galk-opensource/krembot_sim/src/master/ and install it according to the instructions written in the link.
4. **krembot_ws:** This is the workspace of the krembot. You can download it from this repository, or create one of your own by copy the directory "krembot_ws" from "krembot_sim" to a path as you wish.


## Configurations
A mission or experiment are defined by _.argos_ files that are kind of XMLs that setting up the arena, its size, obstacles in it and other experiment preferences. For each mission/experiment/exercise, there is an _.argos_ file in the directory krembot_ws/config. The _.argos_ file is referencing C++ files that defines Krembots' behaviors.

To run an experiment use the command:
> $ argos 3 -c <_config_>
 
where <_config_> should be replaced by the path to the _.argos_ file that defines the experiment.


## Controllers
As was said before, Krembots' behaviors are defined by C++ files. A C++ program that defines behavior of a Krembot for a specific task is named "Controller". A controller usually contains a _.cpp_ source file, a _.h/.hpp_ header file, a _CMakeLists.txt_ file, and maybe other supportive files. Each mission/experiment requires a krembot's controller to make the krembots behave in such a way that they will fulfill their task. The controllers are located in "krembot_ws/controllers". Each experiments has its own controller, referenced by its unique _.argos_ file.


## Exercise 1 - Warmup
This exercise is a warm-up exercise only. It is designed to install the working environment and to make sure all the simulator components are working properly. As part of this exercise, we have implemented a simple controller code that causes the kremboot to drive straight and stop about 15 meters from an obstacle in front.

Experiment's definition:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/krembot_ws/config/ex1.argos

Controller:
> https://github.com/shlomi1993/Intro-to-Robotics/tree/main/krembot_ws/controllers/ex1

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

Experiment's definition: 
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/krembot_ws/config/ex2.argos

Controller:
> https://github.com/shlomi1993/Intro-to-Robotics/tree/main/krembot_ws/controllers/ex2

To run the experiment, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh wanderbot

For the full report:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics_ex2_Report.pdf


## Exercise 3 - Navigation with PRM

In this exercise we have implemented an algorithm that let the Krembot navigate from the starting position (1,1) to the goal position (-2,-2) using Probabilistic Roadmap, or PRM. We will describe the setup and loop functions, the positions sampling method, the distance metric, the use of KNN and KD-Tree data structure, the shortest path algorithm, and the robot's feedback-control. 

Experiment's definition:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/krembot_ws/config/PRM.argos

Controller:
> https://github.com/shlomi1993/Intro-to-Robotics/tree/main/krembot_ws/controllers/PRM

To run the experiment, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh navigation

For the full report:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics_ex3_Report.pdf


## Exercise 4 - Foraging Tournament

In this exercise we have implemented an algorithm that let the Krembot forage in an unknown arena without using its current position. We used this algorithm in an
Adversarial Foraging tournament where we competed with other classmates and won the 2nd place out of 18 teams. Our team number was 1. To improve our algorithm to a level it could compete in the tournament, we used the red team. That is, we have developed another algorithm that is supposed to simulate an opponent and compete with our original algorithm that is now called blue team. When the red team defeated the blue team, we improved the blue team, but when the red team won significantly, we turned it into a blue team and developed a new red team.

**Presentation:** We had to present our strategy and red team to the class sevral days before the tournament, means we could change out strategy after the presentation. Here are the slides:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics_ex4_Presentation.pdf

Experiment's original definition:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/krembot_ws/config/tournament.argos

But for developing our algotirhm using red team we used the custom definition:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/krembot_ws/config/tournament_redteam.argos

Blue Team Controller:
> https://github.com/shlomi1993/Intro-to-Robotics/tree/main/krembot_ws/controllers/foraging_1

Red Team Controller:
> https://github.com/shlomi1993/Intro-to-Robotics/tree/main/krembot_ws/controllers/foraging_redteam

To run the experiment against a naive foraging algorithm, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh tournament

Or to run it against the red team, navigate to krembot_ws directory, and run the command:
> $ scripts/run.sh tournament r

For the full blue team report:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics_ex4_Blue_Team_Report.pdf

For the full blue team report:
> https://github.com/shlomi1993/Intro-to-Robotics/blob/main/reports/Robotics_ex4_Red_Team_Report.pdf


## IDEs, Writers and Tools

1. JetBrains CLION
2. Visual Studio Code
3. Notepad++
4. Oracle VirtualBox VM
5. Windows Subsystem for Linux 2


## Notes
* For your ease, you can use the scripts in the krembot_ws/scripts directory to compile the project and run a simulation easily.
* The simulator can be run on different distributions of Linux as well as on WSL2, but you may need to configure a lot of things for it to work.


## Screenshots

Exercise 2 - Example of a covarage result:

![image](https://user-images.githubusercontent.com/72878018/153854561-7ad0823c-bda4-460a-b51a-36514cbbfc41.png)


Exercise 3 - Example of a navigation result using L2 metric and K=30:

![image](https://user-images.githubusercontent.com/72878018/153854815-7f4d4f05-3905-4517-87ed-bf270f46f2f0.png)

