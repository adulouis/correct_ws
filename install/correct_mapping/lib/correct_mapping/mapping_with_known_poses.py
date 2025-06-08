#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import OccupancyGrid, MapMetaData
import rclpy.time
from sensor_msgs.msg import LaserScan
from tf2_ros import Buffer, TransformListener, LookupException


class Pose:
    def __init__(self, px=0, py=0):
        self.x = px
        self.y = py

def coordinateToPose(x, y, map_info: MapMetaData):
    pose = Pose()
    pose.x = round((x - map_info.origin.position.x)/map_info.resolution)
    pose.x = round((y - map_info.origin.position.y)/map_info.resolution)
    return pose

def poseOnMap(pose: Pose, map_info: MapMetaData):
    return pose.x < map_info.width and pose >= 0 and pose.y < map_info.height and pose.y >= 0

def poseToCell(pose: Pose, map_info: MapMetaData):
    return map_info.width * pose.y + pose.x


class MappingWithKnownPoses(Node):
    def __init__(self, name):
        super().__init__(name)

        self.declare_parameter("width", 50.0)
        self.declare_parameter("height", 50.0)
        self.declare_parameter("resolution", 0.1)
        width = self.get_parameter("width").get_parameter_value().double_value
        height = self.get_parameter("height").get_parameter_value().double_value
        resolution = self.get_parameter("resolution").get_parameter_value().double_value

        self.map = OccupancyGrid()
        self.map.info.resolution = resolution
        self.map.info.width = round(width / resolution)
        self.map.info.height = round(height /resolution)
        self.map.info.origin.position.x = float(-round(width / 2.0))
        self.map.info.origin.position.y = float(-round(height / 2.0))
        self.map.header.frame_id = "odom" #using the odometry of the robot
        self.map.data = [-1] * (self.map.info.width * self.map.info.height)

        self.map_pub = self.create_publisher(OccupancyGrid, "map", 1)
        self.scan_sub = self.create_subscription(LaserScan, "scan", self.scanCallback, 10)
        self.timer = self.create_timer(1.0, self.timerCallback)

        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf_buffer,self)


    def scanCallback(self, scan: LaserScan):
        try:
            transform = self.tf_buffer.lookup_transform(self.map.header.frame_id, scan.header.frame_id, rclpy.time.Time())
        except LookupException:
            self.get_logger().error("Unable to transform between /odom and /base_footprint")
            return
        
        robot_p = coordinateToPose(transform.transform.translation.x, transform.transform.translation.y, self.map.info)
        if not poseOnMap(robot_p, self.map.info): #checking if the robot is in the map
            self.get_logger().error("Robot is not in the map")
            return
        
        robot_cell = poseToCell(robot_p, self.map.info)
        self.map.data[robot_cell] = 100

    def timerCallback(self):
        self.map.header.stamp = self.get_clock().now().to_msg()
        self.map_pub.publish(self.map)


def main():
    rclpy.init()
    node = MappingWithKnownPoses("mapping_with_know_poses")
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == "__main__":
    main()