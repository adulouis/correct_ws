#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#include <chrono>
#include <libserial/SerialPort.h>

using namespace std::chrono_literals;

class SimpleSerialReceiver : public rclcpp::Node
{
public:
  SimpleSerialReceiver() : Node("simple_serial_receiver")
  {
    declare_parameter<std::string>("port", "/dev/ttyUSB0");

    port = get_parameter("port").as_string();

    arduino.Open(port);
    arduino.SetBaudRate(LibSerial::BaudRate::BAUD_115200);

    pub = create_publisher<std_msgs::msg::String>("serial_receiver", 10);
    timer = create_wall_timer(0.01s, std::bind(&SimpleSerialReceiver::timerCallback, this));
  }

  ~SimpleSerialReceiver()
  {
    arduino.Close();
  }

  void timerCallback()
  {
    if(rclcpp::ok() && arduino.IsDataAvailable())
    {
      auto message = std_msgs::msg::String();
      arduino.ReadLine(message.data);
      pub->publish(message);
    }
  }

private:
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr pub;
  rclcpp::TimerBase::SharedPtr timer;
  std::string port;
  LibSerial::SerialPort arduino;
};
