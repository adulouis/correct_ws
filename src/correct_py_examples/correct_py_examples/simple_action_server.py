import rclpy
import time
from rclpy.node import Node
from rclpy.action import ActionServer
from correct_msgs.action import Fibonnaci


class SimpleActionServer(Node):
    def __init__(self):
        super().__init__("simple_action_server")
        self.simple_action = ActionServer(self, Fibonnaci, "fibonnaci", self.goalCallback)
        self.get_logger().info("Starting the action server...")


    def goalCallback(self, goal_handle):
        #Getting the request
        self.get_logger().info(f"Received goal request with order {goal_handle.request.order}")
        feedback_msg = Fibonnaci.Feedback()
        feedback_msg.partial_sequence = [0,1]

        for i in range(1, goal_handle.request.order):
            feedback_msg.partial_sequence.append(feedback_msg.partial_sequence[i] + feedback_msg.partial_sequence[i-1])
            self.get_logger().info(f"Feedback: {feedback_msg.partial_sequence}")
            goal_handle.publish_feedback(feedback_msg)
            time.sleep(1) #because this is time intensive

        goal_handle.succeed()
        result = Fibonnaci.Result()
        result.sequence = feedback_msg.partial_sequence
        return result
    

def main():
    rclpy.init()
    node = SimpleActionServer()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()

