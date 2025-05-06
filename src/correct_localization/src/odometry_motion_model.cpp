#include "correct_localization/odometry_motion_model.hpp"
#include <geometry_msgs/msg/pose.hpp>
#include <tf2/utils.h>

using std::placeholders::_1;


OdometryMotionModel::OdometryMotionModel(const std::string& name) 
                    : Node(name),
                     alpha1(0.0),
                     alpha2(0.0),
                     alpha3(0.0),
                     alpha4(0.0),
                     nr_samples(300),
                     last_odom_x(0.0),
                     last_odom_y(0.0),
                     last_odom_theta(0.0),
                     is_first_odom(true)
{

  declare_parameter("alpha1",0.1);
  declare_parameter("alpha2",0.1);
  declare_parameter("alpha3",0.1);
  declare_parameter("alpha4",0.1);
  declare_parameter("nr_samples",300);
  alpha1 = get_parameter("alpha1").as_double();
  alpha2 = get_parameter("alpha2").as_double();
  alpha3 = get_parameter("alpha3").as_double();
  alpha4 = get_parameter("alpha4").as_double();
  nr_samples = get_parameter("nr_samples").as_int();
  
  if(nr_samples > 0){
    samples.poses = std::vector<geometry_msgs::msg::Pose>(nr_samples, geometry_msgs::msg::Pose());
  }
  else{
    RCLCPP_FATAL_STREAM(get_logger(), "Invalid samples received. Should be "<<nr_samples);
    return;
  }

  odom_sub = create_subscription<nav_msgs::msg::Odometry>("correct_controller/odom", 10, std::bind(&OdometryMotionModel::odomCallback, this, _1));
  pose_array_pub = create_publisher<geometry_msgs::msg::PoseArray>("odometry_motion_model/samples", 10);
}


void OdometryMotionModel::odomCallback(const nav_msgs::msg::Odometry &odom)
{
   tf2::Quaternion q()
}



int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<OdometryMotionModel>("odometry_motion_model");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}