#!/usr/bin/env python3
# Subscribe from the twist mux and publish unto correct_controller/cmd_vel in order to move the robot
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist, TwistStamped


class TwistRelay(Node):
    def __init__(self):
        super().__init__("twist_relay")
        self.controller_sub = self.create_subscription(Twist, "/correct_controller/cmd_vel_unstamped", self.controller_twist_callback, 10)
        self.controller_pub = self.create_publisher(TwistStamped, "/correct_controller/cmd_vel",10)
        self.joy_sub = self.create_subscription(TwistStamped, "/input_joy/cmd_vel_stamped",self.joy_twist_callback,10)
        self.joy_pub = self.create_publisher(Twist, "/input_joy/cmd_vel",10)

    #Add the Stamped to the Twist message from twist_mux
    def controller_twist_callback(self, msg):
        twist_stamped = TwistStamped()
        twist_stamped.header.stamp = self.get_clock().now().to_msg()
        twist_stamped.twist = msg
        self.controller_pub.publish(twist_stamped)

    
    #Convert the twist_stamped velocities on the joystick to twist
    def joy_twist_callback(self, msg):
        twist = Twist()
        twist = msg.twist
        self.joy_pub.publish(twist)


def main():
    rclpy.init()
    node = TwistRelay()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()

