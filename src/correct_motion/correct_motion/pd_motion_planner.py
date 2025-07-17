#!usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Path
from geometry_msgs.msg import Twist,PoseStamped
import rclpy.time
from tf2_ros import Buffer, TransformListener, LookupException


class PDMotionPlanner(Node):
    def __init__(self):
        super().__init__("pd_motion_planner")
        self.declare_parameter("kp",2.0)
        self.declare_parameter("kd",0.1)
        self.declare_parameter("step_size",0.2)
        self.declare_parameter("max_linear_velocity",0.3)
        self.declare_parameter("max_angular_velocity",1.0)

        self.kp = self.get_parameter("kp").value
        self.kd = self.get_parameter("kd").value
        self.step_size = self.get_parameter("step_size").value
        self.mlv = self.get_parameter("max_linear_velocity").value
        self.mav = self.get_parameter("max_angular_velocity").value

        self.path_sub = self.create_subscription(Path, "/a_star/path",self.pathCallback,10)
        self.cmd_pub = self.create_publisher(Twist,"/cmd_vel",10)
        self.next_pose_pub = self.create_publisher(PoseStamped,"/pd/next_pose",10) #publishing the next pose to be moved to by the robot

        self.tf = Buffer()
        self.tf_listener = TransformListener(self.tf, self)
        self.timer = self.create_timer(0.1, self.control_loop)

        self.global_plan = None


    def pathCallback(self, path: Path):
        self.global_plan = path

    def control_loop(self):
        if not self.global_plan or self.global_plan.poses:
            return
        try:
            robot_pose_transform = self.tf.lookup_transform("odom", "base_footprint",rclpy.time.Time())
        except LookupException as e:
            self.get_logger().warn(f"Could not get the position of the robot: {e}")

        #checking the frame id's of the path and the robot
        self.get_logger().info(f"Frame id of the robot: {robot_pose_transform.header.frame_id}")
        self.get_logger().info(f"Frame id of the robot: {self.global_plan.header.frame_id}")


def main():
    rclpy.init()
    node = PDMotionPlanner()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()