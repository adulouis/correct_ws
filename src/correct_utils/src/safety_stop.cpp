#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/bool.hpp"
#include "math.h"


//To keep track of the robot we give a free, warning or danger state
enum State
{
    FREE = 0,
    WARNING = 1,
    DANGER = 2
};

class SafetyStop : public rclcpp::Node
{
    public:
        SafetyStop() : Node("safety_stop_node"), state{State::FREE}
        {
            declare_parameter<double>("danger_distance",0.2);
            declare_parameter<std::string>("scan_topic","scan");
            declare_parameter<std::string>("safety_stop_topic", "safety_stop");
            danger_distance = get_parameter("danger_distance").as_double();
            scan_topic = get_parameter("scan_topic").as_string();
            safety_stop_topic = get_parameter("safety_stop_topic").as_string();

            laser_sub = this->create_subscription<sensor_msgs::msg::LaserScan>(scan_topic, 10,
            std::bind(&SafetyStop::laserCallback, this, std::placeholders::_1));

            safety_stop_pub = this->create_publisher<std_msgs::msg::Bool>(safety_stop_topic, 10);
        }
        
        
    private:
        double danger_distance;
        std::string scan_topic;
        std::string safety_stop_topic;
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr safety_stop_pub;
        State state;

        void laserCallback(const sensor_msgs::msg::LaserScan &msg)
        {
            state = State::FREE;
            for(const auto &range_values: msg.ranges)
            {
                if(!std::isinf(range_values) && range_values <= danger_distance)
                {
                    state = State::DANGER;
                    break;
                }
            }
            std_msgs::msg::Bool is_safet_stop;
            if(state == State::DANGER)
            {
                is_safet_stop.data = true;
            }
            else if(state == State::FREE)
            {
                is_safet_stop.data = false;
            }
            safety_stop_pub->publish(is_safet_stop);
        }
};


int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SafetyStop>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}