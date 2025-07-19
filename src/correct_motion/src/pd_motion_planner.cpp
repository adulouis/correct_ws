#include "correct_motion/pd_motion_planner.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"


namespace correct_motion
{
    PDMotionPlanner::PDMotionPlanner() : Node("pd_motion_planner"), kp(2.0), kd(1.0),
    step_size(0.2),max_linear_velocity(0.3),max_angular_velocity(1.0),last_cycle_time(0.0)
    {
        tf_buffer = std::make_shared<tf2_ros::Buffer>(get_clock());
        tf_listener = std::make_shared<tf2_ros::TransformListener>(*tf_buffer);

        declare_parameter<double>("kp", kp);
        declare_parameter<double>("kd", kd);
        declare_parameter<double>("step_size", step_size);
        declare_parameter<double>("max_linear_velocity", max_linear_velocity);
        declare_parameter<double>("max_angular_velocity", max_angular_velocity);
        kp = get_parameter("kp").as_double();
        kd = get_parameter("kd").as_double();
        step_size = get_parameter("step_size").as_double();
        max_linear_velocity = get_parameter("max_linear_velocity").as_double();
        max_angular_velocity = get_parameter("max_angular_velocity").as_double();

        path_sub = create_subscription<nav_msgs::msg::Path>(
            "/a_star/path", 10, std::bind(&PDMotionPlanner::pathCallback, this, std::placeholders::_1));
                
        cmd_pub = create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 10);

        next_pose_pub = create_publisher<geometry_msgs::msg::PoseStamped>("/pd/next_pose", 10);
        control_loop = create_wall_timer(
            std::chrono::milliseconds(100), std::bind(&PDMotionPlanner::controlLoop, this));
        }

    void PDMotionPlanner::pathCallback(const nav_msgs::msg::Path::SharedPtr path)
    {
        global_plan = *path;
    }

    void PDMotionPlanner::controlLoop()
    {
        if(global_plan.poses.empty())
        {
            return;
        }
        geometry_msgs::msg::TransformStamped robot_pose;
        try
        {
            robot_pose = tf_buffer->lookupTransform("odom","base_footprint",tf2::TimePointZero);
        }
        catch(const tf2::TransformException& e)
        {
            RCLCPP_WARN(get_logger(), "Could not transform: %s", e.what());
            return;
        }
        if(!transformPlan(robot_pose.header.frame_id))
        {
            RCLCPP_ERROR(get_logger(), "Couldn't transform robot poses in robot frame");
            return;
        }
        
    }

    bool PDMotionPlanner::transformPlan(const std::string & frame)
    {
        if(global_plan.header.frame_id == frame)
        {
            return true;
        }

        geometry_msgs::msg::TransformStamped transform;
        try
        {
            transform = tf_buffer->lookupTransform(frame, global_plan.header.frame_id, tf2::TimePointZero);
        }
        catch(const tf2::TransformException& e)
        {
            RCLCPP_ERROR_STREAM(get_logger(), "Couldn't transform from frame "<<global_plan.header.frame_id << "to "<<frame);\
            return false;
        }
        for(auto &pose: global_plan.poses)
        {
            tf2::doTransform(pose, pose, transform);
        }
        global_plan.header.frame_id = frame;
        return true;
        
    }
}


int main(int argv, char* argc[])
{
    rclcpp::init(argv, argc);
    auto node = std::make_shared<correct_motion::PDMotionPlanner>();
    rclcpp::spin(node);
    rclcpp::shutdown();
}