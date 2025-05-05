#ifndef KALMAN_FILTER_HPP
#define KALMAN_FILTER_HPP
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <nav_msgs/msg/odometry.hpp>


class KalmanFilter : public rclcpp::Node
{
    public:
        KalmanFilter(const std::string &name);

        void statePrediction();
        void measurementUpdate();


    private:

        void odomCallback(const nav_msgs::msg::Odometry &odom);
        void imuCallback(const sensor_msgs::msg::Imu &imu);

        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
        rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_sub;
        rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_pub;


        double mean; //initial estimate
        double variance;
        double motion_variance;
        double measurement_variance;
        double motion;
        bool is_first_odom;
        double last_angular_z; //data on the angular vel in z axis from noisy odometry
        double imu_angular_z; //data on the angular vel in z axis from imu sensor    
        nav_msgs::msg::Odometry kalman_odom;

        
        
        
        
};

#endif // KALMAN_FILTER_HPP