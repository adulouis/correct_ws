#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "std_msgs/msg/bool.hpp"
#include "math.h"
#include "rclcpp_action/rclcpp_action.hpp"
#include "twist_mux_msgs/action/joy_turbo.hpp"
#include "visualization_msgs/msg/marker_array.hpp"


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

            is_first_msg = true;

            laser_sub = this->create_subscription<sensor_msgs::msg::LaserScan>(scan_topic, 10,
            std::bind(&SafetyStop::laserCallback, this, std::placeholders::_1));

            safety_stop_pub = this->create_publisher<std_msgs::msg::Bool>(safety_stop_topic, 10);
            zones_pub = this->create_publisher<visualization_msgs::msg::MarkerArray>("zones",10);

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

            visualization_msgs::msg::Marker warning_zone;
            warning_zone.id = 0;
            warning_zone.type = visualization_msgs::msg::Marker::CYLINDER;
            warning_zone.action = visualization_msgs::msg::Marker::ADD;
            warning_zone.scale.z = 0.001;
            warning_zone.scale.x = warning_distance * 2;
            warning_zone.scale.y = warning_distance * 2;
            warning_zone.color.r = 1.0;
            warning_zone.color.g = 0.984;
            warning_zone.color.b = 0.0;
            warning_zone.color.a = 0.5;
            zones.markers.push_back(warning_zone);
            visualization_msgs::msg::Marker danger_zone = warning_zone;
            danger_zone.id = 1;
            danger_zone.scale.x = danger_distance * 2;
            danger_zone.scale.y = danger_distance * 2;
            danger_zone.color.r = 1.0;
            danger_zone.color.g = 0.0;
            danger_zone.color.b = 0.0;
            danger_zone.color.a = 0.5;
            danger_zone.pose.position.z = 0.001;
            zones.markers.push_back(danger_zone);
        }
        
        
    private:
        bool is_first_msg;
        double danger_distance, warning_distance;
        std::string scan_topic;
        std::string safety_stop_topic;
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr laser_sub;
        rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr safety_stop_pub;
        rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr zones_pub;
        rclcpp_action::Client<twist_mux_msgs::action::JoyTurbo>::SharedPtr decrease_speed_client;
        rclcpp_action::Client<twist_mux_msgs::action::JoyTurbo>::SharedPtr increase_speed_client; 
        State state, prev_state;
        visualization_msgs::msg::MarkerArray zones;

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
                    zones.markers[0].color.a = 1.0;
                    zones.markers[1].color.a = 0.5;
                    decrease_speed_client->async_send_goal(twist_mux_msgs::action::JoyTurbo::Goal());
                }
                else if(state == State::DANGER)
                {
                    zones.markers[0].color.a = 1.0;
                    zones.markers[1].color.a = 1.0;
                    is_safet_stop.data = true;
                }
                else if(state == State::FREE)
                {
                    zones.markers[0].color.a = 0.5;
                    zones.markers[0].color.a = 0.5;
                    is_safet_stop.data = false;
                    increase_speed_client->async_send_goal(twist_mux_msgs::action::JoyTurbo::Goal());

                }
                prev_state = state;
                safety_stop_pub->publish(is_safet_stop);
            }

            if(is_first_msg)
            {
                for(auto &zone: zones.markers)
                {
                    zone.header.frame_id = msg.header.frame_id;
                }
                is_first_msg = false;
            }
            zones_pub->publish(zones);
            
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