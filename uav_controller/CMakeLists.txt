cmake_minimum_required(VERSION 2.8.3)
project(uav_controller)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  geometry_msgs
  visualization_msgs
)

include_directories(${catkin_INCLUDE_DIRS})

add_executable(hexacopter_controller src/hexacopter_controller.cpp)

target_link_libraries(hexacopter_controller ${catkin_LIBRARIES})


