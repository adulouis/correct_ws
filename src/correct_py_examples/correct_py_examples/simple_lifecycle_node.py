import rclpy
import time
import rclpy.executors
from rclpy.lifecycle import Node, State, TransitionCallbackReturn
from std_msgs.msg import String


class LifeCycleNode(Node):
    def __init__(self, node_name,**kwargs):
        super().__init__(node_name, **kwargs)

    def on_configure(self, state)->TransitionCallbackReturn:
        self.sub = self.create_subscription(String, "chatter",self.msgCallback,10)
        self.get_logger().info("Lifecylce node on_configure up!")
        return TransitionCallbackReturn.SUCCESS
    
    def on_shutdown(self, state)->TransitionCallbackReturn:
        self.destroy_subscription(self.sub)
        self.get_logger().info("Lifecylce node on_shutdown up!")
        return TransitionCallbackReturn.SUCCESS
    
    def on_cleanup(self, state) ->TransitionCallbackReturn:
        self.destroy_subscription(self.sub)
        self.get_logger().info("Lifecycle node on_cleanup up!")
        return TransitionCallbackReturn.SUCCESS
    
    def on_activate(self, state: State):
        self.get_logger().info("Lifecycle node on_activate up!")
        time.sleep(2)
        return super().on_activate(state)
    
    def on_deactivate(self, state: State):
        self.get_logger().info("Lifecycle node on_deactivate up!")
        return super().on_deactivate(state)
    
    def msgCallback(self, msg):
        current__state = self._state_machine.current_state
        if current__state[1] == "activate":
            self.get_logger().info(f"I heard {msg.data}")


def main():
    rclpy.init()
    executor = rclpy.executors.SingleThreadedExecutor()
    simple_lifecycle_node = LifeCycleNode("simple_lifecycle_node")
    executor.add_node(simple_lifecycle_node)
    try:
        executor.spin()
    except (KeyboardInterrupt, rclpy.executors.ExternalShutdownException):
        simple_lifecycle_node.destroy_node()


if __name__ == "__main__":
    main()