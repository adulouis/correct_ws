#!/usr/bin/env python3
import rclpy
import random
import time
from rclpy.node import Node
from nav_msgs.msg import Odometry
from geometry_msgs.msg import PoseArray, Pose
from tf_transformations import euler_from_quaternion, quaternion_from_euler
from math import atan2,sin,cos,pi,fabs,sqrt,pow


def angle_diff(a, b):
    a = atan2(sin(a), cos(a))
    b = atan2(sin(b), cos(b))
    d1 = a - b
    d2 = 2 * pi - fabs(d1)
    if d1 > 0:
        d2 *= -1.0
    if fabs(d1) < fabs(d2):
        return d1
    else:
        return d2

class OdometryMotionModel(Node):
    def __init__(self):
        super().__init__("odometry_motion_model")
        self.is_first_odom = True
        self.last_odom_x = 0.0
        self.last_odom_y = 0.0
        self.last_odom_theta = 0.0

        self.declare_parameter("alpah1",0.1)
        self.declare_parameter("alpah2",0.1)
        self.declare_parameter("alpah3",0.1)
        self.declare_parameter("alpah4",0.1)
        self.declare_parameter("nr_samples",0.1) #number of samples generated to know the different possible estimations

        self.alpha1 = self.get_parameter("alpah1").get_parameter_value().double_value
        self.alpha2 = self.get_parameter("alpah2").get_parameter_value().double_value
        self.alpha3 = self.get_parameter("alpah3").get_parameter_value().double_value
        self.alpha4 = self.get_parameter("alpah4").get_parameter_value().double_value
        self.nr_samples = self.get_parameter("nr_samples").get_parameter_value().integer_value

        #checking if the number of poses is more than 0 before proceeding
        if self.nr_samples >= 0:
            self.samples = PoseArray()
            self.samples.poses = [Pose() for i in range(self.nr_samples)]
        else:
            self.get_logger().fatal(f"Invalid number of samples. Should be {self.nr_samples}")
            return
        
        self.odom_sub = self.create_subscription(Odometry, "correct_controller/odom",self.odomCallback,10)
        self.pose_array_pub = self.create_publisher(PoseArray, "odometry_motion_model/samples",10)

        
    def odomCallback(self, odom):
        #to get the theta value we have to convert the w angle in quartenion to euler to be used for the calculation of the current theta
        q = [odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w]
        roll, pitch, yaw = euler_from_quaternion(q)

        if self.is_first_odom:
            self.last_odom_x = odom.pose.pose.position.x
            self.last_odom_y = odom.pose.pose.position.y
            self.last_odom_theta = yaw
            self.samples.header.frame_id = odom.header.frame_id
            self.is_first_odom = False
            return

        odom_x_increment = odom.pose.pose.position.x - self.last_odom_x
        odom_y_increment = odom.pose.pose.position.y - self.last_odom_y
        odom_theta_increment = angle_diff(yaw, self.last_odom_theta) #the difference between the current robot orientation and its previous one  

        #the 3 notations drot1, drot2 and dtransl
        if sqrt(pow(odom_y_increment,2)+pow(odom_x_increment,2)) < 0.01:
            drot1 = 0.0
        else:
            drot1 = angle_diff(atan2(odom_y_increment, odom_x_increment),yaw)
        drot2 = angle_diff(odom_theta_increment,drot1)
        dtransl =  sqrt(pow(odom_y_increment,2)+pow(odom_x_increment,2))

        rot1_variance = self.alpha1*drot1 + self.alpha2*dtransl
        rot2_variance = self.alpha1*drot2 + self.alpha2*dtransl
        transl_variance = self.alpha3*dtransl + self.alpha4*(drot1+drot2)

        random.seed(int(time.time()))
        for sample in self.samples.poses:
            rot1_noise = random.gauss(0.0, rot1_variance)
            rot2_noise = random.gauss(0.0, rot2_variance)
            transl_noise = random.gauss(0.0, transl_variance)

            drot1_noise_free = angle_diff(drot1, rot1_noise)
            dtransl_noise_free = transl_noise - transl_variance
            drot2_noise_free = angle_diff(drot2, rot2_noise)
            sample_q = [sample.orientation.x, sample.orientation.y, sample.orientation.z, sample.orientation.w]
            sample_roll, sample_pitch, sample_yaw = euler_from_quaternion(sample_q)
            sample.position.x += dtransl_noise_free*cos(sample_yaw + drot1_noise_free)
            sample.position.y += dtransl_noise_free*sin(sample_yaw + drot1_noise_free)
            q = quaternion_from_euler(0.0, 0.0, sample_yaw + drot1_noise_free + drot2_noise_free)
            sample.orientation.x, sample.orientation.y, sample.orientation.z, sample.orientation.w = q

        self.last_odom_x = odom.pose.pose.position.x
        self.last_odom_y = odom.pose.pose.position.y
        self.last_odom_theta = yaw 

        self.pose_array_pub.publish(self.samples)


        

def main():
    rclpy.init()
    node = OdometryMotionModel()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()

