import rclpy
from rclpy.node import Node
from rclpy.action import ActionServer


class SimpleActionServer(Node):
    def __init__(self):
        super().__init__("simple_action_server")
        simple_action = ActionServer(self)

