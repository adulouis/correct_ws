#include "correct_localization/odometry_motion_model.hpp"

using std::placeholders::_1;


OdometryMotionModel::OdometryMotionModel(const std::string& name) 
                    : Node(name),
                      mean(0.0),
                      variance(1000.0),
                      motion_variance(4.0),
                      measurement_variance(0.5),
                      motion(0.0),
                      is_first_odom(true),
                      last_angular_z(0.0),
                      imu_angular_z(0.0)
{
  odom_sub = create_subscription<nav_msgs::msg::Odometry>("correct_controller/odom_noisy", 10, std::bind(&KalmanFilter::odomCallback, this, _1));
  odom_pub = create_publisher<nav_msgs::msg::Odometry>("correct_controller/odom_kalman", 10);
}


void KalmanFilter::odomCallback(const nav_msgs::msg::Odometry &odom)
{
  kalman_odom = odom;

  if(is_first_odom)
  {
      last_angular_z = odom.twist.twist.angular.z;
      mean = odom.twist.twist.angular.z;
      is_first_odom = false;
      return;
  }

    motion = odom.twist.twist.angular.z - last_angular_z;

    statePrediction();
    measurementUpdate();

    // Update for the next iteration
    last_angular_z = odom.twist.twist.angular.z;

    // Update and publish the filtered odom message
    kalman_odom.twist.twist.angular.z = mean;
    odom_pub->publish(kalman_odom);
}


void KalmanFilter::imuCallback(const sensor_msgs::msg::Imu &imu)
{
    imu_angular_z = imu.angular_velocity.z;
}


void KalmanFilter::measurementUpdate()
{
    mean = (measurement_variance * mean + variance * imu_angular_z)
          / (variance + measurement_variance);

    variance = (variance * measurement_variance) 
              / (variance + measurement_variance);
}


void KalmanFilter::statePrediction()
{
    mean = mean + motion;
    variance = variance + motion_variance;
}


int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<KalmanFilter>("kalman_filter");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}