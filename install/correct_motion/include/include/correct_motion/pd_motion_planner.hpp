#ifndef PD_MOTION_PLANNER_HPP
#define PD_MOTION_PLANNER_HPP

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/path.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"

namespace correct_motion
{
class PDMotionPlanner : public rclcpp::Node
{
public:
    PDMotionPlanner();

private:
    rclcpp::TimerBase::SharedPtr control_loop;
    rclcpp::Subscription<nav_msgs::msg::Path>::SharedPtr path_sub;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub;
    rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr next_pose_pub;
    
    std::shared_ptr<tf2_ros::Buffer> tf_buffer;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener;

    double kp;
    double kd;
    double step_size;
    double max_linear_velocity;
    double max_angular_velocity;

    rclcpp::Time last_cycle_time;

    nav_msgs::msg::Path global_plan;

    void controlLoop();

    bool transformPlan(const std::string & frame);

    void pathCallback(const nav_msgs::msg::Path::SharedPtr path);

    geometry_msgs::msg::PoseStamped getNextPose(
        const geometry_msgs::msg::PoseStamped & robot_pose);
};
}  // namespace bumperbot_motion

#endif // PD_MOTION_PLANNER_HPP