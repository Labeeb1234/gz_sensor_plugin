cmake_minimum_required(VERSION 3.8)
project(sensor_gui_plugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(gz-gui7 REQUIRED)
find_package(gz-transport12 REQUIRED)
find_package(gz-msgs9 REQUIRED)
find_package(gz-plugin2 REQUIRED COMPONENTS register)
find_package(Qt5 REQUIRED COMPONENTS Core Quick QuickControls2)

qt5_wrap_cpp(MOC_SRCS sensor_gui.hh)
qt5_add_resources(RESOURCES SensorParamPlugin.qrc)

add_library(SensorParamPlugin SHARED
  sensor_gui.cc
  ${MOC_SRCS}
  ${RESOURCES}
)

target_link_libraries(SensorParamPlugin
  PRIVATE
    gz-gui7::gz-gui7
    gz-transport12::gz-transport12
    gz-msgs9::gz-msgs9
    gz-plugin2::register
    Qt5::Core
    Qt5::Quick
    Qt5::QuickControls2
)

# Fail at link time instead of at plugin load time
target_link_options(SensorParamPlugin PRIVATE "-Wl,--no-undefined")

target_include_directories(SensorParamPlugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})