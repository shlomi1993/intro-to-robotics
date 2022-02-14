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


### Summery
In this section we'll explain our coverage algorithm and report its success. Generally, the algorithm let the robot wander around randomly in strait lines until most of the arena is covered.


### Algorithm Description
Explanation: The algorithm let the robot drive at full speed until one of its front bumpers pressed, and then stop and change driving angle by turning in a random speed and direction for a random time (up to 3 seconds), and then drive again and so on.
Note: We inferred that it doesn't matter if one of the other bumpers pressed because the driving is forward. We found that the rear bumpers never pressed, but sometimes the right or the left bumpers are pressed. We decided to ignore side presses because the driving is forward so we can save turnings.


### Success Reports


#### _random_seed=1_

End Clock-Ticks: 16,000.

End Screenshot:

![image](https://user-images.githubusercontent.com/72878018/153832880-2ec43a4f-6aee-43be-a2b5-3a361e279689.png)

This result is nice, but not very satisfies because in 10,000 clock-ticks the arena was fairly covered except from the upper-left part of the arena. That is happening because of the concentration of obstacles in the rest of the arena.

Here is a screenshot at 10,014 clock ticks:

![image](https://user-images.githubusercontent.com/72878018/153833226-509f434d-6c48-4b74-b2ec-7ed1c0483fa0.png)

 
### _random_seed=2_

End Clock-Ticks: 15,350.

End Screenshot:

![image](https://user-images.githubusercontent.com/72878018/153833621-a48d14d4-a9ab-4568-b844-ada55a667340.png)
 
This result is pretty satisfying enough because the arena is fairly covered in less clock-ticks then the previous experiment.
We were more satisfied by the fact that the Krembot found no problem to pass between two adjacent obstacles.

Here is a screenshot (from 26,750 clock-ticks):
 
![image](https://user-images.githubusercontent.com/72878018/153833788-f253cdd0-1042-44c2-941f-65106bd9a0f0.png)


### _random_seed=3_

End Clock-Ticks: 18,510.

End Screenshot:

![image](https://user-images.githubusercontent.com/72878018/153833811-836e36e6-80d2-406f-bc55-4926f8acafd4.png)

We were less satisfied by this result than previous results because it took more time for the robot to fairly cover the arena. We assume this happened due to the lack of obstacles at the bottom left part of the arena, similar to what happened in experiment number 1.

 
### _random_seed=4_

End Clock-Ticks: 14,600.

End Screenshot:

![image](https://user-images.githubusercontent.com/72878018/153833875-9a4c44ed-c69f-40fe-a621-69f081453541.png)

We are satisfied by this result because it is the best result we had so far. Note that the robot successfully passed through tight passage between the obstacle on the upper part of the arena to the upper wall.
 

### _random_seed=5_

End Clock-Ticks: 22,660.

End Screenshot:

![image](https://user-images.githubusercontent.com/72878018/153833928-46faa84d-f59c-46b2-b032-7de64284c336.png)

This result is acceptable, but we are not really happy with it because it is the worst result from the 5 experiments. We can see how the robot goes over the same lines over and over again and it wastes time. On one hand this can be prevented by changing the angle of motion at random times. On the other hand, we have tried that and in general it didn't yield us a significantly faster performance, so maybe in another run with different random numbers such a case can be avoided.


### Exercise 3 - Navigation with PRM

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

