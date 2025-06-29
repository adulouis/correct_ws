import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from ament_index_python import get_package_share_directory
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    ros_distro = os.environ["ROS_DISTRO"]
    if ros_distro != "humble":
        lifecycle_nodes.append("slam_toolbox")

    use_sim_time_arg = DeclareLaunchArgument(
        "use_sim_time",
        default_value="true"
    )

    slam_config_arg = DeclareLaunchArgument(
        "slam_config",
        default_value=os.path.join(get_package_share_directory("correct_mapping"), "config", "slam_toolbox.yaml")
    )

    use_sim_time = LaunchConfiguration("use_sim_time")

    slam_config = LaunchConfiguration("slam_config")

    slam_toolbox = Node(
        package="slam_toolbox",
        executable="sync_slam_toolbox_node",
        name="slam_toolbox",
        output="screen",
        parameters=[{"use_sim_time": use_sim_time},
                    slam_config]
    )

    #for saving the map
    nav2_map_server = Node(
        package="nav2_map_server",
        executable="map_saver_server",
        name="map_saver_server",
        output="screen",
        parameters=[
            {"save_map_timeout": 5.0},
            {"use_sim_time": use_sim_time},
            {"free_thresh_default": 0.196},
            {"occupied_thresh_default": 0.65}
        ]
    )

    lifecycle_nodes = ["map_saver_server"]

    #since nav2_map_server node is a lifecycle node and we don't want to manually start it everytime, we use the nav2_lifecycle_manager
    nav2_lifecycle_manager = Node(
        package="nav2_lifecycle_manager",
        executable="lifecycle_manager",
        name="lifecycle_manager",
        output="screen",
        parameters=[{"node_names": lifecycle_nodes},
                    {"use_sim_time": use_sim_time}]
    )

    return LaunchDescription([
        use_sim_time_arg,
        slam_config_arg,
        slam_toolbox,
        nav2_map_server,
        nav2_lifecycle_manager
    ])