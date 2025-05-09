from setuptools import find_packages, setup

package_name = 'correct_py_examples'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='louwee',
    maintainer_email='louwee@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'simple_publisher = correct_py_examples.simple_publisher:main',
            'simple_subscriber = correct_py_examples.simple_subscriber:main',
            'simple_parameter = correct_py_examples.simple_parameter:main',
            'simple_turtlesim_kinematics = correct_py_examples.simple_turtlesim_kinematics:main',
            'simple_service_server = correct_py_examples.simple_service_server:main',
            'simple_service_client = correct_py_examples.simple_service_client:main',
            'simple_tf_kinematics = correct_py_examples.simple_tf_kinematics:main',
            'simple_lifecycle_node = correct_py_examples.simple_lifecycle_node:main',
        ],
    },
)
