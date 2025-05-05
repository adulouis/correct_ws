#!/usr/bin/env python3

import numpy as np
import rclpy
import math
from geometry_msgs.msg import TwistStamped
from rclpy.node import Node
from std_msgs.msg import Float64MultiArray
from sensor_msgs.msg import JointState
from nav_msgs.msg import Odometry
from rclpy.time import Time
from rclpy.constants import S_TO_NS
from transforms3d.euler import euler2quat
from tf2_ros.transform_broadcaster import TransformBroadcaster, TransformStamped

class SimpleController(Node):
    def __init__(self):
        super().__init__("simple_controller")

        self.declare_parameter("wheel_radius",0.033)
        self.declare_parameter("wheel_separation",0.17)
        self.wheel_radius = self.get_parameter("wheel_radius").get_parameter_value().double_value
        self.wheel_separation = self.get_parameter("wheel_separation").get_parameter_value().double_value

        self.get_logger().info(f"Using wheel radius {self.wheel_radius}")
        self.get_logger().info(f"Using wheel separation {self.wheel_separation}")

        ##Variables for the inverse kinematics
        self.left_prev_pos = 0.0
        self.right_prev_pos = 0.0

        self.x = 0.0
        self.y = 0.0
        self.theta = 0.0

        self.wheel_cmd_pub = self.create_publisher(Float64MultiArray, "simple_velocity_controller/commands",10)
        self.vel_sub = self.create_subscription(TwistStamped, "correct_controller/cmd_vel",self.msgCallback,10)
        self.inv_kinematics = self.create_subscription(JointState, "joint_states",self.inv_kinematicsCallback, 10)
        self.odom_pub = self.create_publisher(Odometry,"correct_controller/odom",10)

        self.speed_conversion = np.array([[self.wheel_radius/2, self.wheel_radius/2],
                                          [self.wheel_radius/self.wheel_separation, -self.wheel_radius/self.wheel_separation]])
        
        self.odom_msg = Odometry()
        self.odom_msg.header.frame_id = "odom"
        self.odom_msg.child_frame_id = "base_footprint"
        self.odom_msg.pose.pose.orientation.x = 0.0
        self.odom_msg.pose.pose.orientation.y = 0.0
        self.odom_msg.pose.pose.orientation.z = 0.0
        self.odom_msg.pose.pose.orientation.w = 1.0

        # Fill the TF message
        self.br = TransformBroadcaster(self)
        self.transform_stamped = TransformStamped()
        self.transform_stamped.header.frame_id = "odom"
        self.transform_stamped.child_frame_id = "base_footprint"
        self.prev_time = self.get_clock().now()

        
        self.get_logger().info(f"Speed conversion: {self.speed_conversion}")


    def msgCallback(self, msg):
        #Implement differential kinematics model
        robot_vel = np.array([[msg.twist.linear.x],
                              [msg.twist.angular.z]])
        #Calc the wheel speeds
        wheel_vel = np.matmul(np.linalg.inv(self.speed_conversion),robot_vel)
        wheel_vel_msg = Float64MultiArray()
        wheel_vel_msg.data = [wheel_vel[1,0], wheel_vel[0,0]]
        self.wheel_cmd_pub.publish(wheel_vel_msg)


    def inv_kinematicsCallback(self,msg):
        left_curr_pos = msg.position[1]
        right_curr_pos = msg.position[0]
        left_delta_pos = left_curr_pos - self.left_prev_pos
        right_delta_pos = right_curr_pos - self.right_prev_pos
        delta_time = Time.from_msg(msg.header.stamp) - self.prev_time
        self.left_prev_pos = left_curr_pos
        self.right_prev_pos = right_curr_pos
        self.prev_time = Time.from_msg(msg.header.stamp)

        fi_left = left_delta_pos / (delta_time.nanoseconds/S_TO_NS)
        fi_right = right_delta_pos/(delta_time.nanoseconds/S_TO_NS)

        ##We can now calculate the linear and angular velocities
        linear = (self.wheel_radius*fi_right + self.wheel_radius*fi_left)/2
        angular = (self.wheel_radius*fi_right + self.wheel_radius*fi_left)/self.wheel_separation
        ##To calculate the position of the robot
        ds = (self.wheel_radius*right_delta_pos + self.wheel_radius*left_delta_pos)/2
        d_theta = (self.wheel_radius*right_delta_pos - self.wheel_radius*left_delta_pos)/self.wheel_separation
        self.theta += d_theta
        self.x += ds*math.cos(d_theta)
        self.y += ds*math.cos(d_theta)
        

        q = euler2quat(0,0,self.theta)
        self.odom_msg.pose.pose.orientation.x = q[0]
        self.odom_msg.pose.pose.orientation.y = q[1]
        self.odom_msg.pose.pose.orientation.z = q[2]
        self.odom_msg.pose.pose.orientation.w = q[3]
        self.odom_msg.header.stamp = self.get_clock().now().to_msg()
        self.odom_msg.pose.pose.position.x = self.x
        self.odom_msg.pose.pose.position.y = self.y
        self.odom_msg.twist.twist.linear.x = linear
        self.odom_msg.twist.twist.angular.z = angular

        self.odom_pub.publish(self.odom_msg)

        # TF
        self.transform_stamped.transform.translation.x = self.x
        self.transform_stamped.transform.translation.y = self.y
        self.transform_stamped.transform.rotation.x = q[0]
        self.transform_stamped.transform.rotation.y = q[1]
        self.transform_stamped.transform.rotation.z = q[2]
        self.transform_stamped.transform.rotation.w = q[3]
        self.transform_stamped.header.stamp = self.get_clock().now().to_msg()
        self.br.sendTransform(self.transform_stamped)



def main():
    rclpy.init()
    simple_controller = SimpleController()
    rclpy.spin(simple_controller)
    simple_controller.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()