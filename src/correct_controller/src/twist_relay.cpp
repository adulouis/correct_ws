#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>

using  std::placeholders::_1;


class TwistRelay : public rclcpp::Node
{
    public: TwistRelay() : Node("twist_relay")
    {
        controller_sub = create_subscription<geometry_msgs::msg::Twist>("/correct_controller/cmd_vel_unstamped",10,std::bind(&TwistRelay::controller_twist_callback, this, _1));
        controller_pub = create_publisher<geometry_msgs::msg::TwistStamped>("/correct_controller/cmd_vel",10);
        joy_sub = create_subscription<geometry_msgs::msg::TwistStamped>("/input_joy/cmd_vel_stamped",10,std::bind(&TwistRelay::joy_twist_callback,this,_1));
        joy_pub = create_publisher<geometry_msgs::msg::Twist>("/input_joy/cmd_vel",10);

    }

    private:
        rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr controller_sub;
        rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr joy_sub;
        rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr controller_pub;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr joy_pub;

        geometry_msgs::msg::Twist twist;
        geometry_msgs::msg::TwistStamped twist_stamped;


        void controller_twist_callback(const geometry_msgs::msg::Twist &msg)
        {
            twist_stamped.header.stamp = get_clock()->now();
            twist_stamped.twist = msg;
            controller_pub->publish(twist_stamped);
        }

        void joy_twist_callback(const geometry_msgs::msg::Twist &msg)
        {
            twist = msg;
            joy_pub->publish(twist);
        }
    };


int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TwistRelay>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}