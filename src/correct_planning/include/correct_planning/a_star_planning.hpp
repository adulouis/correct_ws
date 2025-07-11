#ifndef ASTAR_PLANNER_HPP
#define ASTAR_PLANNER_HPP

#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "nav_msgs/msg/path.hpp"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"

namespace correct_planning{

struct GraphNode
{
    int x;
    int y;
    int cost;
    double heuristic;
    std::shared_ptr<GraphNode> prev;

    GraphNode() : GraphNode(0,0) {}

    GraphNode(int in_x, int in_y) : x(in_x), y(in_y), cost(0){}

    bool operator>(const GraphNode & other) const { 
        return cost + heuristic > other.cost + other.heuristic;
    }

    bool operator==(const GraphNode & other) const {
        return x == other.x && y == other.y;
    }

    GraphNode operator+(std::pair<int, int> const & other) {
        GraphNode res(x + other.first, y + other.second);
        return res;
    }
};

class AStarPlanner : public rclcpp::Node
{
    public:
        AStarPlanner();

    private:
        rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr map_sub;
        rclcpp::Subscription<geometry_msgs::msg::PoseStamped>::SharedPtr pose_sub;
        rclcpp::Publisher<nav_msgs::msg::OccupancyGrid>::SharedPtr map_pub;
        rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr path_pub;

        nav_msgs::msg::OccupancyGrid::SharedPtr map;
        nav_msgs::msg::OccupancyGrid visited_map;

        std::shared_ptr<tf2_ros::Buffer> tf_buffer;
        std::shared_ptr<tf2_ros::TransformListener> tf_listener;

        void mapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr map_msg);

        void goalCallback(const geometry_msgs::msg::PoseStamped::SharedPtr goal_msg);

        nav_msgs::msg::Path plan(const geometry_msgs::msg::Pose &start, const geometry_msgs::msg::Pose &end);

        bool poseOnMap(const GraphNode & node);

        GraphNode worldToGrid(const geometry_msgs::msg::Pose & pose);

        geometry_msgs::msg::Pose gridToWorld(const GraphNode & node);

        unsigned int poseToCell(const GraphNode & node);

        double manhattanDistance(const GraphNode &start_node, const GraphNode &goal_node);

};
}
#endif // DIJKSTRA_PLANNER_HPP