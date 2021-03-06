#include <ros/ros.h>
#include <trajectory_msgs/MultiDOFJointTrajectory.h>
#include <geometry_msgs/PoseStamped.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sensor_msgs/Joy.h>
#include <visualization_msgs/Marker.h>
using namespace std;

//create a file to store the true positions of the drone
ofstream myfile("true_position.txt", ios::out | ios::binary);                                                           

void rgbd_cb(const visualization_msgs::Marker rgbd_msg);
void pose_cb (const geometry_msgs::PoseStamped pose_msg);
void joy_cb(const sensor_msgs::Joy joy_msg);

double height=1 ;
double x_axis=0 ;
double y_axis=0 ;
double x,y,z,quaternion_x,quaternion_y,quaternion_z,quaternion_w;
int received = 0;

trajectory_msgs::MultiDOFJointTrajectory velocityMsg;

ros::Subscriber rgbdSub;
ros::Subscriber poseSub;
ros::Subscriber joySub;
ros::Publisher  pub;

int main (int argc, char** argv) 
{

	// Initialize ROS
	ros::init (argc, argv, "controller_firefly");
	ros::NodeHandle nh("~");

	//subscribing on rgbd estimated position
	rgbdSub = nh.subscribe<visualization_msgs::Marker>("/rgbdslam/pose_graph_markers",1,rgbd_cb);

	//subscribing on firefly odometry
	poseSub = nh.subscribe<geometry_msgs::PoseStamped>("/firefly/odometry_sensor1/pose",1,pose_cb);
	
	//subscribing on joystick
	joySub = nh.subscribe<>("/joy",1,joy_cb);
	
	pub = nh.advertise<trajectory_msgs::MultiDOFJointTrajectory>("/firefly/command/trajectory",1);

	//Filling the velocity vector message
	velocityMsg.header.stamp = ros::Time::now();
	velocityMsg.points.resize(1);
	velocityMsg.points[0].transforms.resize(1);
	velocityMsg.points[0].velocities.resize(1);
	velocityMsg.points[0].transforms[0].translation.x=x;
	velocityMsg.points[0].transforms[0].translation.y=y;
	velocityMsg.points[0].transforms[0].translation.z=height;
	velocityMsg.points[0].transforms[0].rotation.x = quaternion_x;
	velocityMsg.points[0].transforms[0].rotation.y = quaternion_y;
	velocityMsg.points[0].transforms[0].rotation.z = quaternion_z;
	velocityMsg.points[0].transforms[0].rotation.w = quaternion_w;
	
	pub.publish(velocityMsg);

	ros::spinOnce();
	
	ros::Rate rate(100);

	while (ros::ok())
	{ 
	       ros::spinOnce();
               rate.sleep();
	}
}

//advertise pose_cb that data is received from RGB-D algorithm, the estimated positions are stored in a different file provided by the GUI of the algorithm
void rgbd_cb(const visualization_msgs::Marker rgbd_msg)                                                                 
{                                                                                                                       
        received =1;                                                                                                    
} 

void pose_cb (const geometry_msgs::PoseStamped pose_msg) 
{  
//store the true position and orientation
	x=pose_msg.pose.position.x;
	y=pose_msg.pose.position.y;
	z=pose_msg.pose.position.z;
	quaternion_x=pose_msg.pose.orientation.x;
	quaternion_y=pose_msg.pose.orientation.y;
	quaternion_z=pose_msg.pose.orientation.z;
	quaternion_w=pose_msg.pose.orientation.w;

//if data from RGBD algorithm is received store the true position in myFile, this mechanism allows the synchronization of data frequency between rue position and the estimated one
        if ( received == 1 )                                                                                            
        {	                                                                                                          
            myfile<<"  X= "<< x <<" Y= "<< y <<" Z= "<< z <<"\n";                                                       
            received = 0;
        }  
}

//control the drone by position through a joystick
void joy_cb (const sensor_msgs::Joy joy_msg)                                                                            
{
	float ahead = joy_msg.buttons[4];
	float retro = joy_msg.buttons[6];
	float right = joy_msg.buttons[5];
	float left = joy_msg.buttons[7];
	float up = joy_msg.buttons[12];
	float down = joy_msg.buttons[14];
	float roll_clockwise = joy_msg.buttons[13];
	float roll_counterClockwise = joy_msg.buttons[15];

        velocityMsg.points[0].velocities[0].linear.x=0;
        velocityMsg.points[0].velocities[0].linear.z=0;
        velocityMsg.points[0].velocities[0].angular.z=0;

        if ( ahead > 0.5 )
            x_axis = x_axis+0.01;
 
        if ( retro > 0.5 )
            x_axis = x_axis-0.01;

        if ( right > 0.5 )
            y_axis = y_axis + 0.01; 

        if ( left > 0.5 )
            y_axis = y_axis - 0.01;

        if ( up > 0.5 )
            height = height + 0.01;
        
        if ( down > 0.5 )
            height = height - 0.01;
     
        if ( roll_clockwise > 0.5 )
            velocityMsg.points[0].velocities[0].angular.z=0.01;

        if ( roll_counterClockwise > 0.5 )
            velocityMsg.points[0].velocities[0].angular.z=-0.01;

        
        velocityMsg.points.resize(1);
        velocityMsg.points[0].transforms.resize(1);
        velocityMsg.points[0].velocities.resize(1);
        velocityMsg.points[0].transforms[0].translation.x=x_axis;
        velocityMsg.points[0].transforms[0].translation.y=y_axis;
	velocityMsg.points[0].transforms[0].translation.z=height;
        velocityMsg.points[0].transforms[0].rotation.x = quaternion_x;
	velocityMsg.points[0].transforms[0].rotation.y = quaternion_y;
	velocityMsg.points[0].transforms[0].rotation.z = quaternion_z;
	velocityMsg.points[0].transforms[0].rotation.w = quaternion_w;

        pub.publish(velocityMsg);
}
