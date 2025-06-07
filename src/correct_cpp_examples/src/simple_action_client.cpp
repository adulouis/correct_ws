#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <rclcpp_components/register_node_macro.hpp>
#include "correct_msgs/action/fibonacci.hpp"


using namespace std::placeholders;
namespace correct_cpp_examples{
class SimpleActionClient : public rclcpp::Node
{
    public:
        explicit SimpleActionClient(const rclcpp::NodeOptions &options = rclcpp::NodeOptions())
        :Node("simple_action_client", options)
        {
            client = rclcpp_action::create_client<correct_msgs::action::Fibonacci>(this,"fibonacci");
            timer = create_wall_timer(std::chrono::seconds(1), std::bind(&SimpleActionClient::timerCallback,this));
        }


    private:
        rclcpp_action::Client<correct_msgs::action::Fibonacci>::SharedPtr client;
        rclcpp::TimerBase::SharedPtr timer;

        void timerCallback()
        {
            timer->cancel();
            if(!client->wait_for_action_server())
            {
                RCLCPP_INFO(get_logger(), "Action Server not available after waiting");
                rclcpp::shutdown();
            }
            auto goal = correct_msgs::action::Fibonacci::Goal();
            goal.order = 20;
            RCLCPP_INFO(get_logger(),"Sending goal");

            auto send_goal_options = rclcpp_action::Client<correct_msgs::action::Fibonacci>::SendGoalOptions();
            send_goal_options.goal_response_callback = std::bind(&SimpleActionClient::goalCallback,this,_1);
            send_goal_options.feedback_callback = std::bind(&SimpleActionClient::feedbackCallback,this,_1,_2);
            send_goal_options.result_callback = std::bind(&SimpleActionClient::resultCallback,this,_1);
            client->async_send_goal(goal, send_goal_options);
        }

        void goalCallback(const rclcpp_action::ClientGoalHandle<correct_msgs::action::Fibonacci>::SharedPtr &goal_handle)
        {
            if(!goal_handle)
            {
                RCLCPP_INFO(get_logger(), "Goal was Rejected");
            }
            else{
                RCLCPP_INFO(get_logger(), "Goal was Accepted");
            }
        }

        void feedbackCallback(rclcpp_action::ClientGoalHandle<correct_msgs::action::Fibonacci>::SharedPtr, 
        const std::shared_ptr<const correct_msgs::action::Fibonacci::Feedback> feedback)
        {
            std::stringstream ss;
            ss <<"Next number in the sequence received: ";
            for(auto i: feedback->partial_sequence)
            {
                ss<<i<<" ";
            }
            RCLCPP_INFO(get_logger(), ss.str().c_str());
        }

        void resultCallback(const rclcpp_action::ClientGoalHandle<correct_msgs::action::Fibonacci>::WrappedResult result)
        {
            switch(result.code)
            {
                case rclcpp_action::ResultCode::SUCCEEDED:
                    break;
                case rclcpp_action::ResultCode::ABORTED:
                    RCLCPP_ERROR(get_logger(), "Goal was aborted");
                    return;
                case rclcpp_action::ResultCode::CANCELED:
                    RCLCPP_ERROR(get_logger(), "Goal was cancelled");
                    return;
                default:
                    RCLCPP_ERROR(get_logger(), "Unknown result code");
                    return;
            }
            std::stringstream ss;
            ss <<"Result received: ";
            for(auto i: result.result->sequence)
            {
                ss<<i<<" ";
            }
            RCLCPP_INFO(get_logger(), ss.str().c_str());
            rclcpp::shutdown();
        }
};
}
RCLCPP_COMPONENTS_REGISTER_NODE(correct_cpp_examples::SimpleActionClient)