#ifndef MAPPING_WITH_KNOWN_POSES_HPP
#define MAPPING_WITH_KNOWN_POSES_HPP

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "tf2_ros/transform_listener.h"
#include "tf2_ros/buffer.h"


namespace correct_mapping{
struct Pose{
    Pose() = default;
    Pose(const int px, const int py) : x(px), y(py){}
    int x;
    int y;
};

unsigned int poseToCell(const Pose &pose, const nav_msgs::msg::MapMetaData &map_info); //transform the pose of the robot to be drawn on the map

Pose coordinateToPose(const double px, const double py, const nav_msgs::msg::MapMetaData &map_info);

bool poseOnMap(const Pose &pose, const nav_msgs::msg::MapMetaData &map_info);

class MappingWithKnownPoses : public rclcpp::Node
{
    public:
        MappingWithKnownPoses(const std::string &name);


    private:
        void scanCallback(const sensor_msgs::msg::LaserScan &scan);

        void timerCallback();

        nav_msgs::msg::OccupancyGrid map;
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub;
        rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr map_pub;
        rclcpp::TimerBase::SharedPtr timer;
        std::unique_ptr<tf2_ros::Buffer> tf_buffer;
        std::shared_ptr<tf2_ros::TransformListener> tf_listener{nullptr};
};
}
#endif