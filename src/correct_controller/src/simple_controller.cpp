#include "correct_controller/simple_controller.hpp"
#include <Eigen/Geometry>


using std::placeholders::_1;

SimpleController::SimpleController(const std::string& name)
                                  : Node(name),
                                  left_prev_pos(0.0),
                                  right_prev_pos(0.0),
                                  x(0.0),
                                  y(0.0),
                                  theta(0.0)
{
    declare_parameter("wheel_radius", 0.033);
    declare_parameter("wheel_separation", 0.17);
    wheel_radius = get_parameter("wheel_radius").as_double();
    wheel_separation = get_parameter("wheel_separation").as_double();
    RCLCPP_INFO_STREAM(get_logger(), "Using wheel radius " << wheel_radius);
    RCLCPP_INFO_STREAM(get_logger(), "Using wheel separation " << wheel_separation);
    wheel_pub_cmd = create_publisher<std_msgs::msg::Float64MultiArray>("/simple_velocity_controller/commands", 10);
    vel_cmd = create_subscription<geometry_msgs::msg::TwistStamped>("/correct_controller/cmd_vel", 10, std::bind(&SimpleController::msgCallback, this, _1));
    inv_kinematics = create_subscription<sensor_msgs::msg::JointState>("/joint_states",10, std::bind(&SimpleController::inv_kinematicsCallback, this, _1));
    odom_pub = create_publisher<nav_msgs::msg::Odometry>("/correct_controller/odom", 10);
    

    speed_conversion << wheel_radius/2, wheel_radius/2, wheel_radius/wheel_separation, -wheel_radius/wheel_separation;
    RCLCPP_INFO_STREAM(get_logger(), "The conversion matrix is \n" << speed_conversion);

    // Fill the Odometry message with invariant parameters
    odom_msg.header.frame_id = "odom";
    odom_msg.child_frame_id = "base_footprint";
    odom_msg.pose.pose.orientation.x = 0.0;
    odom_msg.pose.pose.orientation.y = 0.0;
    odom_msg.pose.pose.orientation.z = 0.0;
    odom_msg.pose.pose.orientation.w = 1.0;

    transform_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(*this);
    transform_stamped.header.frame_id = "odom";
    transform_stamped.child_frame_id = "base_footprint";

    prev_time = get_clock()->now();
}


void SimpleController::msgCallback(const geometry_msgs::msg::TwistStamped &msg)
{
    // Implements the differential kinematic model
    // Given v and w, calculate the velocities of the wheels
    Eigen::Vector2d robot_speed(msg.twist.linear.x, msg.twist.angular.z);
    Eigen::Vector2d wheel_speed = speed_conversion.inverse() * robot_speed;
    std_msgs::msg::Float64MultiArray wheel_speed_msg;
    wheel_speed_msg.data.push_back(wheel_speed.coeff(1));
    wheel_speed_msg.data.push_back(wheel_speed.coeff(0));
    
    wheel_pub_cmd->publish(wheel_speed_msg);
}

void SimpleController::inv_kinematicsCallback(const sensor_msgs::msg::JointState &msg)
{
  double delta_left = msg.position.at(1) - left_prev_pos;
  double delta_right = msg.position.at(0) - right_prev_pos;
  rclcpp::Time msg_time = msg.header.stamp;
  rclcpp::Duration delta_time = msg_time - prev_time;   

  left_prev_pos = msg.position.at(1);
  right_prev_pos = msg.position.at(0);
  prev_time = msg.header.stamp;

  double fi_left = delta_left/delta_time.seconds();
  double fi_right = delta_right/delta_time.seconds();

  double linear = (wheel_radius * fi_right + wheel_radius* fi_left)/2;
  double angular = (wheel_radius * fi_right - wheel_radius* fi_left)/wheel_separation;

  double ds = (wheel_radius * delta_right + wheel_radius* delta_left)/2;
  double d_theta = (wheel_radius * delta_right + wheel_radius* delta_left)/wheel_separation;
  theta+=d_theta;
  x += ds*cos(theta);
  y += ds*sin(theta);
  


  // Compose and publish the odom message
  tf2::Quaternion q;
  q.setRPY(0, 0, theta);
  odom_msg.header.stamp = get_clock()->now();
  odom_msg.pose.pose.position.x = x;
  odom_msg.pose.pose.position.y = y;
  odom_msg.pose.pose.orientation.x = q.getX();
  odom_msg.pose.pose.orientation.y = q.getY();
  odom_msg.pose.pose.orientation.z = q.getZ();
  odom_msg.pose.pose.orientation.w = q.getW();
  odom_msg.twist.twist.linear.x = linear;
  odom_msg.twist.twist.angular.z = angular;
  odom_pub->publish(odom_msg);

  // TF
  transform_stamped.transform.translation.x = x;
  transform_stamped.transform.translation.y = y;
  transform_stamped.transform.rotation.x = q.getX();
  transform_stamped.transform.rotation.y = q.getY();
  transform_stamped.transform.rotation.z = q.getZ();
  transform_stamped.transform.rotation.w = q.getW();
  transform_stamped.header.stamp = get_clock()->now();
  transform_broadcaster->sendTransform(transform_stamped);


}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<SimpleController>("simple_controller");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
