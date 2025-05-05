#ifndef SIMPLE_CONTROLLER_HPP
#define SIMPLE_CONTROLLER_HPP

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <Eigen/Core>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2/LinearMath/Quaternion.h>


class SimpleController : public rclcpp::Node
{
    public:
        SimpleController(const std::string &name);

    private:
        void msgCallback(const geometry_msgs::msg::TwistStamped &msg);
        void inv_kinematicsCallback(const sensor_msgs::msg::JointState &msg);

        rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr wheel_pub_cmd;
        rclcpp::Subscription<geometry_msgs::msg::TwistStamped>::SharedPtr vel_cmd;
        rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr inv_kinematics;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub;

        double wheel_radius;
        double wheel_separation;
        double left_prev_pos;
        double right_prev_pos;
        double x;
        double y;
        double theta;
        rclcpp::Time prev_time;
        Eigen::Matrix2d speed_conversion;
        nav_msgs::msg::Odometry odom_msg;

        // TF
        std::unique_ptr<tf2_ros::TransformBroadcaster> transform_broadcaster;
        geometry_msgs::msg::TransformStamped transform_stamped;
};

#endif // SIMPLE_CONTROLLER_HPP