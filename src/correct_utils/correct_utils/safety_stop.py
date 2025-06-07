#!/usr/bin/env python3
import rclpy
import math
from rclpy.node import Node
from rclpy.action import ActionClient
from sensor_msgs.msg import LaserScan
from std_msgs.msg import Bool
from enum import Enum
from twist_mux_msgs.action import JoyTurbo
from visualization_msgs.msg import Marker, MarkerArray


class State(Enum):
    FREE = 0
    WARNING = 1
    DANGER = 2


class SafetyStop(Node):
    def __init__(self):
        super().__init__("safety_stop_node")
        self.state = State.FREE
        self.prev_state = State.FREE

        self.declare_parameter("danger_distance", 0.2)
        self.declare_parameter("warning_distance", 0.7)
        self.declare_parameter("scan_topic", "scan")
        self.declare_parameter("safety_stop_topic", "safety_stop")
        self.danger_distance = self.get_parameter("danger_distance").get_parameter_value().double_value
        self.warning_distance = self.get_parameter("warning_distance").get_parameter_value().double_value
        self.scan_topic = self.get_parameter("scan_topic").get_parameter_value().string_value
        self.safety_stop_topic = self.get_parameter("safety_stop_topic").get_parameter_value().string_value
        self.is_first_msg = True
        self.laser_sub = self.create_subscription(LaserScan, self.scan_topic, self.laserCallback, 10)
        self.safety_stop_pub = self.create_publisher(Bool, self.safety_stop_topic, 10)
        self.zones_pub = self.create_publisher(MarkerArray, "zones", 10)

        self.decrease_speed_client = ActionClient(self, JoyTurbo,"joy_turbo_decrease")
        self.increase_speed_client = ActionClient(self, JoyTurbo,"joy_turbo_increase")

        while not self.decrease_speed_client.wait_for_server(timeout_sec=1.0) and rclpy.ok():
            self.get_logger().warn("Action /joy_turbo_decrease not available! waiting...")

        while not self.increase_speed_client.wait_for_server(timeout_sec=1.0) and rclpy.ok():
            self.get_logger().warn("Action /joy_turbo_increase not available! waiting...")

        self.zones = MarkerArray()
        warning_zone = Marker()
        warning_zone.id = 0
        warning_zone.type = Marker.CYLINDER
        warning_zone.action = Marker.ADD
        warning_zone.scale.z = 0.001
        warning_zone.scale.x = self.warning_distance * 2
        warning_zone.scale.y = self.warning_distance * 2
        warning_zone.color.r = 1.0
        warning_zone.color.g = 0.984
        warning_zone.color.b = 0.0
        warning_zone.color.a = 0.5
        danger_zone = Marker()
        danger_zone.id = 1
        danger_zone.type = Marker.CYLINDER
        danger_zone.action = Marker.ADD
        danger_zone.scale.z = 0.001
        danger_zone.scale.x = self.danger_distance * 2
        danger_zone.scale.y = self.danger_distance * 2
        danger_zone.color.r = 1.0
        danger_zone.color.g = 0.0
        danger_zone.color.b = 0.0
        danger_zone.color.a = 0.5
        danger_zone.pose.position.z = 0.01

        self.zones.markers = [warning_zone, danger_zone]


    def laserCallback(self, msg):
        self.state = State.FREE
        for range_value in msg.ranges:
            if not math.isinf(range_value) and range_value <= self.warning_distance:
                self.state = State.WARNING
                if range_value <= self.danger_distance:
                    self.state = State.DANGER
                    break

        if self.state != self.prev_state:
            is_safety_stop = Bool()
            if self.state == State.WARNING:
                is_safety_stop.data = False
                self.zones.markers[0].color.a = 1.0
                self.zones.markers[1].color.a = 0.5
                self.decrease_speed_client.send_goal_async(JoyTurbo.Goal())
            if self.state == State.DANGER:
                self.zones.markers[0].color.a = 1.0
                self.zones.markers[1].color.a = 1.0
                is_safety_stop.data = True
            elif self.state == State.FREE:
                self.zones.markers[0].color.a = 0.5
                self.zones.markers[1].color.a = 0.5
                is_safety_stop.data = False
                self.decrease_speed_client.send_goal_async(JoyTurbo.Goal())
                
            self.prev_state = self.state
            self.safety_stop_pub.publish(is_safety_stop)

        if self.is_first_msg:
            for zone in self.zones.markers:
                zone.header.frame_id = msg.header.frame_id

            self.is_first_msg = False

        self.zones_pub.publish(self.zones)

        


def main():
    rclpy.init()
    node = SafetyStop()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()


        
