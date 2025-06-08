import rclpy
from rclpy.node import Node
from std_msgs.msg import String
from rclpy.qos import QoSDurabilityPolicy, QoSProfile, QoSReliabilityPolicy


class SimpleQoSSubscriber(Node):

    def __init__(self):
        super().__init__("simple_subscriber")
        self.qos_profile_sub = QoSProfile(depth=10)

        self.declare_parameter("reliability", "system_default")
        self.declare_parameter("durability", "system_default")

        reliability = self.get_parameter("reliability").get_parameter_value().string_value
        durability = self.get_parameter("durability").get_parameter_value().string_value

        if reliability == "best_effort":
            self.qos_profile_sub.reliability = QoSReliabilityPolicy.BEST_EFFORT
            self.get_logger().info("[Reliability] : Best Effort")
        elif reliability == "reliability":
            self.qos_profile_sub.reliability = QoSReliabilityPolicy.RELIABLE
            self.get_logger().info("[Reliability] : Reliable")
        elif reliability == "system_default":
            self.qos_profile_sub.reliability = QoSReliabilityPolicy.RELIABLE
            self.get_logger().info("[Reliability] : System Default")
        else:
            self.get_logger().error(f"Selected Reliablity QoS doesnt exist {reliability}")
            return
        
        if durability == "volatility":
            self.qos_profile_sub.durability = QoSDurabilityPolicy.VOLATILE
            self.get_logger().info("[Durability] : Volatile")
        elif durability == "transient_local":
            self.qos_profile_sub.durability = QoSDurabilityPolicy.TRANSIENT_LOCAL
            self.get_logger().info("[Durability] : Transient Local")
        elif durability == "system_default":
            self.qos_profile_sub.durability = QoSDurabilityPolicy.SYSTEM_DEFAULT
            self.get_logger().info("[Durability] : System Default")
        else:
            self.get_logger().error(f"Selected Reliablity QoS doesnt exist {reliability}")
            return

        self.sub_ = self.create_subscription(String, "chatter", self.msgCallback, self.qos_profile_sub)
        self.sub_

    def msgCallback(self, msg):
        self.get_logger().info("I heard: %s" % msg.data)


def main():
    rclpy.init()

    simple_qos_subsciber = SimpleQoSSubscriber()
    rclpy.spin(simple_qos_subsciber)
    
    simple_qos_subsciber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()