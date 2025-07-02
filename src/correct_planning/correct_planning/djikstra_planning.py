#!/usr/bin/env/ python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import OccupancyGrid, Path
from rclpy.qos import DurabilityPolicy, QoSProfile
from geometry_msgs.msg import PoseStamped, Pose
import rclpy.time
from tf2_ros import Buffer, TransformListener, LookupException


class DjikstraPlanner(Node):
    def __init__(self):
        super().__init__("djikstra_node")
        self.map_qos = QoSProfile(depth=10)
        self.map_qos.reliability = DurabilityPolicy.TRANSIENT_LOCAL
        self.map_sub = self.create_subscription(OccupancyGrid, "/map", self.mapCallback,self.map_qos)
        self.poses_sub = self.create_subscription(PoseStamped, "/goal_pose", self.goalCallback, 10)
        self.path_pub = self.create_publisher(Path, "/dijkstra/path",10)
        self.map_pub = self.create_publisher(OccupancyGrid, "/dijkstra/visited_map",10)

        self.map = None
        self.visited_map = OccupancyGrid()
        self.buffer = Buffer()
        self.transform = TransformListener(self.buffer, self)
        self.poses = PoseStamped()

    
    def mapCallback(self, map_msg: OccupancyGrid):
        self.map = map_msg
        if self.map is None:
            self.get_logger().error("No map received")
            return
        self.visited_map.header.frame_id = self.map.header.frame_id
        self.visited_map.info = self.map.info
        self.visited_map.data = [-1] * (self.map.info.width * self.map.info.height)

    
    def goalCallback(self, pose: PoseStamped):
        if self.map is None:
            self.get_logger().error("No map received")
            return
        self.visited_map.data = [-1] * (self.visited_map.info.width * self.visited_map.info.height)
        try:
            map_to_robot_tf = self.buffer.lookup_transform(self.map.header.frame_id, "base_footprint", rclpy.time.Time())
        except LookupException:
            self.get_logger().error("Unable to transform between map and robot")
        map_to_robot_pose = Pose()
        map_to_robot_pose.position.x = map_to_robot_tf.transform.translation.x
        map_to_robot_pose.position.y = map_to_robot_tf.transform.translation.y
        map_to_robot_pose.orientation = map_to_robot_tf.transform.rotation

        path = self.djikstraPath(map_to_robot_pose, pose.pose)
        if path is not None:
            self.get_logger().info("Shortest Path found!")
            self.path_pub.publish(path)
        else:
            self.get_logger().error("No path found!")

    def djikstraPath(self, start: Pose, end: Pose):
        pass 
        
        
        
