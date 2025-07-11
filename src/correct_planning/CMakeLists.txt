cmake_minimum_required(VERSION 3.8)
project(correct_planning)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)
find_package(rclpy REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)

include_directories(include)

set(dependencies
  rclcpp
  nav_msgs
  geometry_msgs
  tf2_ros
)

add_executable(djikstra_planning src/djikstra_planning.cpp)
ament_target_dependencies(djikstra_planning ${dependencies})
add_executable(a_star_planning src/a_star_planning.cpp)
ament_target_dependencies(a_star_planning ${dependencies})


install(
  DIRECTORY include
  DESTINATION include
)

ament_python_install_package(${PROJECT_NAME})

install(TARGETS
  djikstra_planning
  a_star_planning
  DESTINATION lib/${PROJECT_NAME}
)

install(PROGRAMS
  ${PROJECT_NAME}/djikstra_planning.py
  ${PROJECT_NAME}/a_star_planning.py
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_dependencies(${dependencies})
ament_package()
