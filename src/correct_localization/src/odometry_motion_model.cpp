#include "correct_localization/odometry_motion_model.hpp"
#include <geometry_msgs/msg/pose.hpp>
#include <tf2/utils.h>
#include <cmath>
#include <random>

using std::placeholders::_1;

double angle_diff(double a, double b){
  a = atan2(sin(a), cos(a));
  b = atan2(sin(b),cos(b));
  double d1 = a - b;
  double d2 = 2 * M_PI - fabs(d1);
  if (d1 > 0) {
    d2 *= -1.0;
  }
  if (fabs(d1) < fabs(d2)) {
    return d1;
  }
  else{
    return d2;
  }
}

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
{  //Converting the quaternion to euler angles 
  tf2::Quaternion q(odom.pose.pose.orientation.x, odom.pose.pose.orientation.y, odom.pose.pose.orientation.z,
    odom.pose.pose.orientation.w);
  tf2::Matrix3x3 m(q);
  double roll, pitch, yaw;
  m.getRPY(roll, pitch, yaw);
  
  if(is_first_odom){
    last_odom_x = odom.pose.pose.position.x;
    last_odom_y = odom.pose.pose.position.y;
    last_odom_theta = yaw;
    samples.header.frame_id = odom.header.frame_id;
    is_first_odom = false;
    return;
  }

  double odom_x_increment = odom.pose.pose.position.x - last_odom_x;
  double odom_y_increment = odom.pose.pose.position.y - last_odom_y;
  double odom_theta_increment = angle_diff(yaw, last_odom_theta);

  double drot1(0.0);

  if (sqrt(std::pow(odom_y_increment,2) + std::pow(odom_x_increment,2)) < 0.01)
  {
    drot1 = 0.0;
  }
  else{
    drot1 = angle_diff(atan2(odom_y_increment,odom_x_increment),yaw);
  }
  double drot2 = angle_diff(odom_theta_increment,drot1);
  double dtransl = sqrt(std::pow(odom_y_increment,2) + std::pow(odom_x_increment,2));

  double rot1_variance = alpha1*drot1 + alpha2*dtransl;
  double rot2_variance = alpha1*drot2 + alpha2*dtransl;
  double transl_variance = alpha3*dtransl + alpha4*(drot1 + drot2);

  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine noise_generator(seed);
  std::normal_distribution<double>rot1_noise(0.0, rot1_variance);
  std::normal_distribution<double>rot2_noise(0.0, rot2_variance);
  std::normal_distribution<double>transl_noise(0.0, transl_variance);

  for(auto & sample : samples.poses){
    double drot1_noise_free = angle_diff(drot1, rot1_noise(noise_generator));
    double dtransl_noise_free = dtransl - transl_noise(noise_generator);
    double drot2_noise_free = angle_diff(drot2, rot2_noise(noise_generator));

    tf2::Quaternion sample_q(sample.orientation.x, sample.orientation.x, sample.orientation.x,
      sample.orientation.x);
    tf2::Matrix3x3 sample_m(sample_q);

    double sample_roll, sample_pitch, sample_yaw;
    sample_m.getRPY(sample_roll, sample_pitch, sample_yaw);
    sample.position.x += dtransl_noise_free * std::cos(sample_yaw + drot1_noise_free);
    sample.position.y += dtransl_noise_free * std::sin(sample_yaw + drot1_noise_free);
    //converting the euler angle to quaternion so we can publish it
    tf2::Quaternion q;
    q.setRPY(0.0, 0.0, sample_yaw + drot1_noise_free + drot2_noise_free);
    sample.orientation.x = q.getX();
    sample.orientation.y = q.getY();
    sample.orientation.z = q.getZ();
    sample.orientation.w = q.getW();

  }

  last_odom_x = odom.pose.pose.position.x;
  last_odom_y = odom.pose.pose.position.y;
  last_odom_theta = yaw;

  pose_array_pub->publish(samples);

}



int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<OdometryMotionModel>("odometry_motion_model");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}