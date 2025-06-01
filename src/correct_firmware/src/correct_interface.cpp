#include "correct_firmware/correct_interface.hpp"
#include <hardware_interface/types/hardware_interface_type_values.hpp>


namespace correct_firmware{

    CorrectInterface::CorrectInterface()
    {

    }

    CorrectInterface::~CorrectInterface()
    {
        if(arduino.IsOpen()){
            try
            {
                arduino.Close();
            }
            catch(...)
            {
                RCLCPP_FATAL_STREAM(rclcpp::get_logger("CorrectInterface"), "Something went wrong while closing the port " << port);
            }
            
        }
    }

    CallbackReturn CorrectInterface::on_init(const hardware_interface::HardwareInfo &hardware_info)
    {
        //getting the hardware info if it fails don't proceed
        CallbackReturn result = hardware_interface::SystemInterface::on_init(hardware_info);
        if(result != CallbackReturn::SUCCESS)
        {
            return result;
        }

        //initialize comms with arduino
        try
        {
            port = info_.hardware_parameters.at("port");
        }
        catch(const std::out_of_range &e)
        {
            RCLCPP_FATAL(rclcpp::get_logger("CorrectInterface"), "No port found");
            return CallbackReturn::FAILURE;
            
        }

        velocity_commands.reserve(info_.joints.size());
        velocity_states.reserve(info_.joints.size());
        position_states.reserve(info_.joints.size());
        last_run = rclcpp::Clock().now();

        return CallbackReturn::SUCCESS;
    }

    std::vector<hardware_interface::StateInterface> CorrectInterface::export_state_interfaces()
    {
        std::vector<hardware_interface::StateInterface> state_interfaces;
        //for the position and velocity states
        for(size_t i = 0; i < info_.joints.size(); i++)
        {
            state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, 
            hardware_interface::HW_IF_POSITION, &position_states.at(i)));

            state_interfaces.emplace_back(hardware_interface::StateInterface(info_.joints[i].name, 
            hardware_interface::HW_IF_VELOCITY, &velocity_states[i]));
        }

        return state_interfaces;
    }

    std::vector<hardware_interface::CommandInterface> CorrectInterface::export_command_interfaces()
    {
        std::vector<hardware_interface::CommandInterface> command_interface;
        for(size_t i = 0; i < info_.joints.size(); i++)
        {
            command_interface.emplace_back(hardware_interface::CommandInterface(info_.joints[i].name, 
            hardware_interface::HW_IF_VELOCITY, &velocity_commands[i]));
        }

        return command_interface;
    }

    CallbackReturn CorrectInterface::on_activate(const rclcpp_lifecycle::State &) 
    {
        RCLCPP_INFO(rclcpp::get_logger("CorrectInterface"), "Starting hardware of the robot...");
        velocity_commands = {0.0, 0.0};
        velocity_states = {0.0, 0.0};
        position_states = {0.0, 0.0};

        try
        {
            arduino.Open(port);
            arduino.SetBaudRate(LibSerial::BaudRate::BAUD_1152000);
        }
        catch(...)
        {
            RCLCPP_FATAL_STREAM(rclcpp::get_logger("CorrectInterface"), "Unable to connect to port "<<port);
            return CallbackReturn::FAILURE;
        }
        RCLCPP_INFO(rclcpp::get_logger("CorrectInterface"), "Hardware started...");
        return CallbackReturn::SUCCESS;
        
    }

    CallbackReturn CorrectInterface::on_deactivate(const rclcpp_lifecycle::State &)
    {
        RCLCPP_INFO(rclcpp::get_logger("CorrectInterface"), "Stopping hardware of the robot...");
        if(arduino.IsOpen()){
            try
            {
                arduino.Close();
            }
            catch(...)
            {
                RCLCPP_FATAL_STREAM(rclcpp::get_logger("CorrectInterface"), "Something went wrong while closing the port " << port);
                return CallbackReturn::FAILURE;
            }
        }
        return CallbackReturn::SUCCESS;
    }

    hardware_interface::return_type CorrectInterface::read(const rclcpp::Time &, const rclcpp::Duration &)
    {
        if(arduino.IsDataAvailable()){
            auto dt = (rclcpp::Clock().now() - last_run).seconds();
            std::string message; //create a string message to read the arduino data
            arduino.ReadLine(message); //the string data is in the form rn2.59 where r is right wheel, n negative and 2.59 is in rad/s
            std::stringstream ss(message); //enable manipulation of string message from the arduino
            std::string res;
            int multiplier = 1; //checking for anticlockwise or clockwise
            while(std::getline(ss, res, ',')) //storing the string of ss into res with separator of ,
            {
                multiplier = res.at(1) == 'p' ? 1 : -1;
                if(res.at(0)=='r')
                {
                    velocity_states.at(0) = multiplier * std::stod(res.substr(2, res.size()));
                    position_states.at(0) += velocity_states.at(0) * dt;
                }
                else if(res.at(0) == 'l')
                {
                    velocity_states.at(1) = multiplier * std::stod(res.substr(2, res.size()));
                    position_states.at(1) += velocity_states.at(1) * dt;
                }
            }
            last_run = rclcpp::Clock().now();
        }
        return hardware_interface::return_type::OK;
    }

    hardware_interface::return_type CorrectInterface::write(const rclcpp::Time &, const rclcpp::Duration &)
    {
        std::stringstream message_stream;
        char right_wheel_sign = velocity_commands.at(0) >= 0 ? 'p' : 'n';
        char left_wheel_sign = velocity_commands.at(1) >= 0 ? 'p' : 'n';
        std::string compensate_zeros_right = "";
        std::string compensate_zeros_left = "";

        if (std::abs(velocity_commands.at(0)) < 10)
        {
            compensate_zeros_right = "0";
        }
        else
        {
            compensate_zeros_right = "";
        }

        if (std::abs(velocity_commands.at(1)) < 10)
        {
            compensate_zeros_left = "0";
        }
        else
        {
            compensate_zeros_left = "";
        }

        message_stream << std::fixed <<std::setprecision(2) << "r" <<right_wheel_sign <<compensate_zeros_right << std::abs(velocity_commands.at(0)) <<
        ",l"<<left_wheel_sign<<compensate_zeros_left<<std::abs(velocity_commands.at(1))<<",";

        try
        {
            arduino.Write(message_stream.str());
        }
        catch(...)
        {
            RCLCPP_ERROR_STREAM(rclcpp::get_logger("CorrectInterface"), "Something went wrong while sending message "
        <<message_stream.str() <<" on " <<port);
        return hardware_interface::return_type::ERROR;
        }
        return hardware_interface::return_type::OK;
    }
}

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(correct_firmware::CorrectInterface, hardware_interface::SystemInterface);