#!/usr/bin/env/ python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import OccupancyGrid, Path
from rclpy.qos import DurabilityPolicy, QoSProfile
from geometry_msgs.msg import PoseStamped, Pose
import rclpy.time
from tf2_ros import Buffer, TransformListener, LookupException
from queue import PriorityQueue


class GraphNode:
    def __init__(self, x, y, cost=0, prev=None):
        self.x = x
        self.y = y
        self.cost = cost
        self.prev = prev

    def __lt__(self, other):
        return self.cost < other.cost
    
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y
    
    def __hash__(self):
        return hash(self.x, self.y)
    
    def __add__(self, other):
        return GraphNode(self.x + other[0], self.y + other[1])


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
        explore_directions = [(-1,0),(1,0),(0,1),(0,-1)] 
        pending_nodes = PriorityQueue()
        visited_nodes = set() #To check if the node has been processed before
        start_node = self.world_to_grid(start)
        pending_nodes.put(start_node)

        #exploring the nodes
        while not pending_nodes.empty() and rclpy.ok():
            active_node = pending_nodes.get()
            if active_node == self.world_to_grid(end):
                break
            
            for dir_x, dir_y in explore_directions:
                new_node: GraphNode = active_node + (dir_x, dir_y)
                if new_node not in visited_nodes and self.pose_in_map(new_node) and self.map.data[self.pose_to_cell(new_node)!=0]:
                    new_node.cost = active_node.cost + 1
                    new_node.prev = active_node
                    pending_nodes.put(new_node)
                    visited_nodes.add(new_node)
            self.visited_map.data[self.pose_to_cell(last_pose)] = 20
            self.map_pub.publish(self.visited_map)

        path = Path()
        path.header.frame_id = self.map.header.frame_id
        while active_node and active_node.prev and rclpy.ok():
            last_pose: Pose = self.grid_to_world(active_node)
            last_pose_stamped = PoseStamped()
            last_pose_stamped.header.frame_id = self.map.header.frame_id
            last_pose_stamped.pose = last_pose
            path.poses.append(last_pose_stamped)
            active_node = active_node.prev
        
        path.poses.reverse()
        return path
            

    def world_to_grid(self, pose: Pose) ->GraphNode:
        grid_x = int((pose.position.x - self.map.info.origin.position.x) / self.map.info.resolution)
        grid_y = int((pose.position.y - self.map.info.origin.position.y) / self.map.info.resolution)
        return GraphNode(grid_x, grid_y)

    def pose_in_map(self, node: GraphNode):
        return 0<= node.x < self.map.info.width and 0<= node.y < self.map.info.width
    
    def pose_to_cell(self, node: GraphNode):
        return node.y * self.map.info.width + node.x
    
    def grid_to_world(self, node: GraphNode):
        pose = Pose()
        pose.position.x = node.x * self.map.info.resolution + self.map.info.origin.position.x
        pose.position.y = node.x * self.map.info.resolution + self.map.info.origin.position.y
        return pose


def main():
    rclpy.init()
    node = DjikstraPlanner()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == "__main__":
    main()        
        
