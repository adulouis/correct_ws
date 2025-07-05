#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "nav_msgs/msg/path.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

namespace correct_planning{
class DjikstraPlanner : public rclcpp::Node
{
    public:
        DjikstraPlanner();

    private:
        rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_sub;
        rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_sub;
        rclcpp::QoS map_qos;
        rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr map_pub;
        rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub;

        nav_msgs::msg::OccupancyGrid::SharedPtr map;
        nav_msgs::msg::OccupancyGrid visited_map;

        std::shared_ptr<tf2_ros::Buffer> tf_buffer;
        std::shared_ptr<tf2_ros::TransformListener> tf_listener;

        void mapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr map_msg);

        void goalCallback(const geometry_msgs::msg::PoseStamped::SharedPtr goal_msg);

        nav_msgs::msg::Path plan(const geometry_msgs::msg::Pose &start, const geometry_msgs::msg::Pose &end);
};
}
