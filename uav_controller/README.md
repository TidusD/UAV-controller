A ROS/Gazebo UAV controller.

To install:
```
$ cd <catkin_ws>/src
$ git clone https://github.com/RGBDslam-joystick/UAV_controller
$ git clone https://github.com/ros-drivers/joystick_drivers
$ cd ..
$ catkin_make
```
How to use:
Joystick controller button-movement: 
	-analog left button: left
	-analog right button: right
	-analog up button: ahead
	-analog down button: back
	-triangle button: up
	-cross button: down	
	-square button: turn left
	-circle button: turn right


If you want to use this controller with the RGB-D SLAM algorithm you have also to install $ git clone https://github.com/felixendres/rgbdslam_v2

The file hexacopter_controller.cpp has a callBack called rgbd_cb which guarantees the synchronous acquisition of data between the true positions of the drone given by the topic "/firefly/odometry_sensor1/pose" and the estimated positions given by the RGB-D algorithm. The first ones are stored in the true_position.txt file, the latter ones in the estimated.txt file provided by the GUI of the RGB-D algorithm.
                     

