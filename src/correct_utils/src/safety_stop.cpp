#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/bool.hpp"
#include "math.h"
#include "rclcpp_action/rclcpp_action.hpp"
#include "twist_mux_msgs/action/joy_turbo.hpp"


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
        SafetyStop() : Node("safety_stop_node"), state{State::FREE}, prev_state{State::FREE}
        {
            declare_parameter<double>("danger_distance",0.2);
            declare_parameter<double>("warning_distance",0.7);
            declare_parameter<std::string>("scan_topic","scan");
            declare_parameter<std::string>("safety_stop_topic", "safety_stop");
            danger_distance = get_parameter("danger_distance").as_double();
            warning_distance = get_parameter("warning_distance").as_double();
            scan_topic = get_parameter("scan_topic").as_string();
            safety_stop_topic = get_parameter("safety_stop_topic").as_string();

            laser_sub = this->create_subscription<sensor_msgs::msg::LaserScan>(scan_topic, 10,
            std::bind(&SafetyStop::laserCallback, this, std::placeholders::_1));

            safety_stop_pub = this->create_publisher<std_msgs::msg::Bool>(safety_stop_topic, 10);

            decrease_speed_client = rclcpp_action::create_client<twist_mux_msgs::action::JoyTurbo>(this, "joy_turbo_decrease");
            increase_speed_client = rclcpp_action::create_client<twist_mux_msgs::action::JoyTurbo>(this, "joy_turbo_increase");
            while(!decrease_speed_client->wait_for_action_server(std::chrono::seconds(1))&&rclcpp::ok())
            {
                RCLCPP_WARN(get_logger(),"Action /joy_turbo_decrease not available! waiting...");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            while(!increase_speed_client->wait_for_action_server(std::chrono::seconds(1))&&rclcpp::ok())
            {
                RCLCPP_WARN(get_logger(),"Action /joy_turbo_increase not available! waiting...");
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        
        
    private:
        double danger_distance, warning_distance;
        std::string scan_topic;
        std::string safety_stop_topic;
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr safety_stop_pub;
        rclcpp_action::Client<twist_mux_msgs::action::JoyTurbo>::SharedPtr decrease_speed_client;
        rclcpp_action::Client<twist_mux_msgs::action::JoyTurbo>::SharedPtr increase_speed_client; 
        State state, prev_state;

        void laserCallback(const sensor_msgs::msg::LaserScan &msg)
        {
            state = State::FREE;
            for(const auto &range_values: msg.ranges)
            {
                if(!std::isinf(range_values) && range_values <= warning_distance)
                {
                    state = State::WARNING;
                    if(range_values <= danger_distance)
                    {
                        state = State::DANGER;
                        break;
                    }
                }
            }

            if(state != prev_state)
            {
                std_msgs::msg::Bool is_safet_stop;
                if(state == State::WARNING)
                {
                    is_safet_stop.data = false;
                    decrease_speed_client->async_send_goal(twist_mux_msgs::action::JoyTurbo::Goal());
                }
                else if(state == State::DANGER)
                {
                    is_safet_stop.data = true;
                }
                else if(state == State::FREE)
                {
                    is_safet_stop.data = false;
                    increase_speed_client->async_send_goal(twist_mux_msgs::action::JoyTurbo::Goal());

                }
                prev_state = state;
                safety_stop_pub->publish(is_safet_stop);
            }
            
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