cmake_minimum_required(VERSION 3.8)
project(sensor_data_plugin)

# Set compiler flags for modern C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Gazebo Sim and related dependencies
find_package(gz-sim7 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(gz-common5 REQUIRED)
find_package(gz-msgs9 REQUIRED)
find_package(gz-transport12 REQUIRED)

set(GZ_SIM_VER 7)
set(GZ_PLUGIN_VER 2)
set(GZ_COMMON_VER 5)
set(GZ_MSGS_VER 9)
set(GZ_TRANSPORT_VER 12)

# Define your custom plugin shared library
add_library(TestSensorDataPlugin SHARED
    sensor_data.cc
)

# Link against all required Gazebo libraries
target_link_libraries(TestSensorDataPlugin
    PRIVATE
        gz-sim${GZ_SIM_VER}::gz-sim${GZ_SIM_VER}
        gz-plugin${GZ_PLUGIN_VER}::gz-plugin${GZ_PLUGIN_VER}
        gz-common${GZ_COMMON_VER}::gz-common${GZ_COMMON_VER}
        gz-msgs${GZ_MSGS_VER}::gz-msgs${GZ_MSGS_VER}
        gz-transport${GZ_TRANSPORT_VER}::gz-transport${GZ_TRANSPORT_VER}
)