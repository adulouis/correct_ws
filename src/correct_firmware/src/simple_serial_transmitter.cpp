#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>

#include <libserial/SerialPort.h>

using std::placeholders::_1;

class SimpleSerialTransmitter : public rclcpp::Node
{
public:
  SimpleSerialTransmitter() : Node("simple_serial_transmitter")
  {
    declare_parameter<std::string>("port", "/dev/ttyUSB0");

    port = get_parameter("port").as_string();

    sub = create_subscription<std_msgs::msg::String>(
        "serial_transmitter", 10, std::bind(&SimpleSerialTransmitter::msgCallback, this, _1));
    
    arduino.Open(port);
    arduino.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
  }

  ~SimpleSerialTransmitter()
  {
    arduino.Close();
  }

  void msgCallback(const std_msgs::msg::String &msg)
  {
    RCLCPP_INFO_STREAM(get_logger(), "New message received on serial_transmitter "<<msg.data);
    arduino.Write(msg.data);
  }

private:
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub;
  std::string port;
  LibSerial::SerialPort arduino;
};

int main(int argc, char* argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SimpleSerialTransmitter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}