#include "correct_planning/correct_planning.hpp"
#include "tf2/utils.h"


namespace correct_planning
{
    DjikstraPlanner::DjikstraPlanner() : Node("djikstra_node"), map_qos(10)
    {
        tf_buffer = std::make_unique<tf2_ros::Buffer>(this->get_clock());
        tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);
        map_qos.durability(RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
        map_sub = create_subscription<nav_msgs::msg::OccupancyGrid>("/map",map_qos,std::bind(DjikstraPlanner::mapCallback, this, std::placeholders::_1));
        pose_sub = create_subscription<geometry_msgs::msg::PoseStamped>("/goal_pose",10,std::bind(DjikstraPlanner::goalCallback, this, std::placeholders::_1));
        map_pub = create_publisher<nav_msgs::msg::OccupancyGrid>("/djikstra/visited_map",10);
        path_pub = create_publisher<nav_msgs::msg::Path>("/djikstra/path",10);
    }

    void DjikstraPlanner::mapCallback(const nav_msgs::msg::OccupancyGrid::SharedPtr map_msg)
    {
        map = map_msg;
        if (map == nullptr)
        {
            RCLCPP_ERROR_STREAM(get_logger(), "No map received");
            return;
        }
        visited_map.header.frame_id = map->header.frame_id;
        visited_map.info = map->info;
        visited_map.data = std::vector<int8_t>(map->info.height * map->info.width, -1);
    }

    void DjikstraPlanner::goalCallback(const geometry_msgs::msg::PoseStamped::SharedPtr goal_msg)
    {
        if (map == nullptr)
        {
            RCLCPP_ERROR_STREAM(get_logger(), "No map received");
            return;
        }
        visited_map.data = std::vector<int8_t>(visited_map.info.height * visited_map.info.width,-1);
        geometry_msgs::msg::TransformStamped map_to_robot_tf;
        //Getting the transform for the position of the robot relative to the map frame
        try
        {
            map_to_robot_tf = tf_buffer->lookupTransform(map->header.frame_id, "base_footprint", tf2::TimePointZero);
            
        }
        catch(const tf2::TransformException& e)
        {
            RCLCPP_ERROR(get_logger(),"Unable to transform between robot_base and the map");
            return;
        }
        geometry_msgs::msg::Pose map_to_robot_pose;
        map_to_robot_pose.position.x = map_to_robot_tf.transform.translation.x;
        map_to_robot_pose.position.y = map_to_robot_tf.transform.translation.y;
        map_to_robot_pose.orientation = map_to_robot_tf.transform.rotation;

        auto path = plan(map_to_robot_pose, goal_msg->pose);
        if(!path.poses.empty())
        {
            RCLCPP_INFO(get_logger(),"Shortest Path Found!");
            path_pub->publish(path);
        }
        else
        {
            RCLCPP_ERROR(get_logger(), "No path found!");
        }
        
    }

    nav_msgs::msg::Path DjikstraPlanner::plan(const geometry_msgs::msg::Pose &start, const geometry_msgs::msg::Pose &end)
    {

    }
}