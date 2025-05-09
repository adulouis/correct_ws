#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <unistd.h>

using namespace std::chrono_literals;
using std::placeholders::_1;

class OdomSubscriber : public rclcpp::Node{
    public:

        OdomSubscriber(std::string & name) : Node(name){
            odom_sub = this->create_subscription<nav_msgs::msg::Odometry>(
                "/correct_controller/odom",10,
                std::bind(&OdomSubscriber::msgCallback, this, _1)
            );
        }

    private:
        void msgCallback(nav_msgs::msg::Odometry &odom){
            RCLCPP_INFO(this->get_logger(), "Getting my Odom data [%f, %f, %f, %f]", 
                                odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z, odom.pose.pose.orientation.w);
        }

        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
};


class SlowTimer : public rclcpp::Node{
    public:
        SlowTimer(float sleep_time) : Node("slow_timer")
        {
            wait_time = sleep_time;
            timer = this->create_wall_timer(500ms, std::bind(&SlowTimer::timerCallback, this));
        }

    private:
        rclcpp::TimerBase::SharedPtr timer;
        float wait_time;
        void timerCallback(){
            sleep(wait_time);
        }

};


int main(int argc, char* argv[]){
    auto node1 = std::make_shared<OdomSubscriber>();
    
    float sleep_timer = 3.0;
    auto node2 = std::make_shared<SlowTimer>(sleep_timer);

    rclcpp::executors::SingleThreadedExecutor executor;

    executor.add_node(node1);
    executor.add_node(node2);
    executor.spin();
    return 0;
}