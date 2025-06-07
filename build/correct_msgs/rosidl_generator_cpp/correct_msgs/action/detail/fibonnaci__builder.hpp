// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from correct_msgs:action/Fibonnaci.idl
// generated code does not contain a copyright notice

#ifndef CORRECT_MSGS__ACTION__DETAIL__FIBONNACI__BUILDER_HPP_
#define CORRECT_MSGS__ACTION__DETAIL__FIBONNACI__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "correct_msgs/action/detail/fibonnaci__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_Goal_order
{
public:
  Init_Fibonnaci_Goal_order()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::correct_msgs::action::Fibonnaci_Goal order(::correct_msgs::action::Fibonnaci_Goal::_order_type arg)
  {
    msg_.order = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_Goal msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_Goal>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_Goal_order();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_Result_sequence
{
public:
  Init_Fibonnaci_Result_sequence()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::correct_msgs::action::Fibonnaci_Result sequence(::correct_msgs::action::Fibonnaci_Result::_sequence_type arg)
  {
    msg_.sequence = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_Result msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_Result>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_Result_sequence();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_Feedback_partial_sequence
{
public:
  Init_Fibonnaci_Feedback_partial_sequence()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::correct_msgs::action::Fibonnaci_Feedback partial_sequence(::correct_msgs::action::Fibonnaci_Feedback::_partial_sequence_type arg)
  {
    msg_.partial_sequence = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_Feedback msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_Feedback>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_Feedback_partial_sequence();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_SendGoal_Request_goal
{
public:
  explicit Init_Fibonnaci_SendGoal_Request_goal(::correct_msgs::action::Fibonnaci_SendGoal_Request & msg)
  : msg_(msg)
  {}
  ::correct_msgs::action::Fibonnaci_SendGoal_Request goal(::correct_msgs::action::Fibonnaci_SendGoal_Request::_goal_type arg)
  {
    msg_.goal = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_SendGoal_Request msg_;
};

class Init_Fibonnaci_SendGoal_Request_goal_id
{
public:
  Init_Fibonnaci_SendGoal_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Fibonnaci_SendGoal_Request_goal goal_id(::correct_msgs::action::Fibonnaci_SendGoal_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_Fibonnaci_SendGoal_Request_goal(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_SendGoal_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_SendGoal_Request>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_SendGoal_Request_goal_id();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_SendGoal_Response_stamp
{
public:
  explicit Init_Fibonnaci_SendGoal_Response_stamp(::correct_msgs::action::Fibonnaci_SendGoal_Response & msg)
  : msg_(msg)
  {}
  ::correct_msgs::action::Fibonnaci_SendGoal_Response stamp(::correct_msgs::action::Fibonnaci_SendGoal_Response::_stamp_type arg)
  {
    msg_.stamp = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_SendGoal_Response msg_;
};

class Init_Fibonnaci_SendGoal_Response_accepted
{
public:
  Init_Fibonnaci_SendGoal_Response_accepted()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Fibonnaci_SendGoal_Response_stamp accepted(::correct_msgs::action::Fibonnaci_SendGoal_Response::_accepted_type arg)
  {
    msg_.accepted = std::move(arg);
    return Init_Fibonnaci_SendGoal_Response_stamp(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_SendGoal_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_SendGoal_Response>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_SendGoal_Response_accepted();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_GetResult_Request_goal_id
{
public:
  Init_Fibonnaci_GetResult_Request_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  ::correct_msgs::action::Fibonnaci_GetResult_Request goal_id(::correct_msgs::action::Fibonnaci_GetResult_Request::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_GetResult_Request msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_GetResult_Request>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_GetResult_Request_goal_id();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_GetResult_Response_result
{
public:
  explicit Init_Fibonnaci_GetResult_Response_result(::correct_msgs::action::Fibonnaci_GetResult_Response & msg)
  : msg_(msg)
  {}
  ::correct_msgs::action::Fibonnaci_GetResult_Response result(::correct_msgs::action::Fibonnaci_GetResult_Response::_result_type arg)
  {
    msg_.result = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_GetResult_Response msg_;
};

class Init_Fibonnaci_GetResult_Response_status
{
public:
  Init_Fibonnaci_GetResult_Response_status()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Fibonnaci_GetResult_Response_result status(::correct_msgs::action::Fibonnaci_GetResult_Response::_status_type arg)
  {
    msg_.status = std::move(arg);
    return Init_Fibonnaci_GetResult_Response_result(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_GetResult_Response msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_GetResult_Response>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_GetResult_Response_status();
}

}  // namespace correct_msgs


namespace correct_msgs
{

namespace action
{

namespace builder
{

class Init_Fibonnaci_FeedbackMessage_feedback
{
public:
  explicit Init_Fibonnaci_FeedbackMessage_feedback(::correct_msgs::action::Fibonnaci_FeedbackMessage & msg)
  : msg_(msg)
  {}
  ::correct_msgs::action::Fibonnaci_FeedbackMessage feedback(::correct_msgs::action::Fibonnaci_FeedbackMessage::_feedback_type arg)
  {
    msg_.feedback = std::move(arg);
    return std::move(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_FeedbackMessage msg_;
};

class Init_Fibonnaci_FeedbackMessage_goal_id
{
public:
  Init_Fibonnaci_FeedbackMessage_goal_id()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Fibonnaci_FeedbackMessage_feedback goal_id(::correct_msgs::action::Fibonnaci_FeedbackMessage::_goal_id_type arg)
  {
    msg_.goal_id = std::move(arg);
    return Init_Fibonnaci_FeedbackMessage_feedback(msg_);
  }

private:
  ::correct_msgs::action::Fibonnaci_FeedbackMessage msg_;
};

}  // namespace builder

}  // namespace action

template<typename MessageType>
auto build();

template<>
inline
auto build<::correct_msgs::action::Fibonnaci_FeedbackMessage>()
{
  return correct_msgs::action::builder::Init_Fibonnaci_FeedbackMessage_goal_id();
}

}  // namespace correct_msgs

#endif  // CORRECT_MSGS__ACTION__DETAIL__FIBONNACI__BUILDER_HPP_
