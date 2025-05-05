#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from geometry_msgs.msg import PoseArray, Pose
from tf_transformations import euler_from_quaternion
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
        self.odom_pub = self.create_publisher(PoseArray, "odometry_motion_model/samples",10)

        
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
        if sqrt(pow(odom_y_increment,2)+pow(odom_x_increment,2)) < 0.1:
            drot1 = 0.0
        else:
            drot1 = angle_diff(atan2(odom_y_increment, odom_x_increment),yaw)
        drot2 = angle_diff(odom_theta_increment,drot1)
        dtransl =  sqrt(pow(odom_y_increment,2)+pow(odom_x_increment,2))    


def main():
    rclpy.init()
    node = OdometryMotionModel()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()

