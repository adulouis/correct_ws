#!usr/bin/env python3
import rclpy
from rclpy.node import Node
from nav_msgs.msg import Path
from geometry_msgs.msg import Twist,PoseStamped
import rclpy.time
from tf2_ros import Buffer, TransformListener, LookupException
from tf_transformations import quaternion_matrix, concatenate_matrices, quaternion_from_matrix, translation_from_matrix


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

        self.tf_buffer = Buffer()
        self.tf_listener = TransformListener(self.tf, self)
        self.timer = self.create_timer(0.1, self.control_loop)

        self.global_plan = None


    def pathCallback(self, path: Path):
        self.global_plan = path

    def control_loop(self):
        if not self.global_plan or self.global_plan.poses:
            return
        try:
            robot_pose_transform = self.tf_buffer.lookup_transform("odom", "base_footprint",rclpy.time.Time())
        except LookupException as e:
            self.get_logger().warn(f"Could not get the position of the robot: {e}")

        #checking the frame id's of the path and the robot
        if not self.transform_plan(robot_pose_transform.header.frame_id):
            self.get_logger().error("Unable to transform in Robot's frame")
            return

    def transform_plan(self, frame):
        if self.global_plan.header.frame_id == frame:
            return True
        try:
            transform = self.tf_buffer.lookup_transform(frame, self.global_plan.header.frame_id,rclpy.time.Time())
        except LookupException as e:
            self.get_logger().error(f"Couldn't transform plan from frame {self.global_plan.header.frame_id} to {frame}: {e}")
            return False
        #Convert the transform to a transformation matrix
        transformation_matrix = quaternion_matrix([transform.transform.rotation.x,
                                                   transform.transform.rotation.y,transform.transform.rotation.z,
                                                   transform.transform.rotation.w])
        #adding the translation path of the matrix
        transformation_matrix[0][3] = transform.transform.translation.x
        transformation_matrix[1][3] = transform.transform.translation.y

        for pose in self.global_plan.poses:
            pose_matrix = quaternion_matrix([pose.pose.orientation.x,
                                                   pose.pose.orientation.y,pose.pose.orientation.z,
                                                   pose.pose.orientation.w])
            #adding the translation path of the matrix
            pose_matrix[0][3] = pose.pose.position.x
            pose_matrix[1][3] = pose.pose.position.y
            #multiplying the 2 matrices
            transformed_pose = concatenate_matrices(pose_matrix, transformation_matrix)
            [pose.pose.orientation.x,pose.pose.orientation.y,
             pose.pose.orientation.z,
            pose.pose.orientation.w]= quaternion_from_matrix(transformed_pose)
            [pose.pose.position.x, pose.pose.position.y, pose.pose.position.z] = translation_from_matrix(transformed_pose)
            pose.header.frame_id = frame

        self.global_plan.header.frame_id = frame
        return True

def main():
    rclpy.init()
    node = PDMotionPlanner()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()