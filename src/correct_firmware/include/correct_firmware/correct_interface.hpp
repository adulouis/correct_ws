#ifndef CORRECT_INTERFACE_HPP
#define CORRECT_INTERFACE_HPP

#include <rclcpp/rclcpp.hpp>
#include <hardware_interface/system_interface.hpp>
#include <rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp>
#include <rclcpp_lifecycle/state.hpp>
#include <libserial/SerialPort.h>
#include <string>
#include <vector>

namespace correct_firmware{
using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;


class CorrectInterface : public hardware_interface::SystemInterface{
    public:
        CorrectInterface();
        virtual ~CorrectInterface(); //virtual destructor

        virtual CallbackReturn on_activate(const rclcpp_lifecycle::State &previous_state) override; //overriding the default on_activate function of the systeminterface

        virtual CallbackReturn on_deactivate(const rclcpp_lifecycle::State &previous_state) override;

        virtual CallbackReturn on_init(const hardware_interface::HardwareInfo &hardware_info) override;

        virtual std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

        virtual std::vector<hardware_interface::CommandInterface> export_command_interfaces() override;

        virtual hardware_interface::return_type read(const rclcpp::Time &time, const rclcpp::Duration &time_taken) override;

        virtual hardware_interface::return_type write(const rclcpp::Time &time, const rclcpp::Duration &time_taken) override;

    private:
        LibSerial::SerialPort arduino;
        std::string port;
        std::vector<double> velocity_commands;
        std::vector<double> velocity_states;
        std::vector<double> postion_states;

        rclcpp::Time last_run; //time of the last execution of the control loop for calc the velocity and position of the wheels
};
}

#endif 