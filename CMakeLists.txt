
cmake_minimum_required(VERSION 3.8)
project(local_map_publisher)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(local_map_publisher
  src/local_map_publisher.cpp
)

target_include_directories(local_map_publisher PRIVATE
  ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(local_map_publisher
  yaml-cpp
  ${OpenCV_LIBRARIES}
)

ament_target_dependencies(local_map_publisher
  rclcpp
  nav_msgs
  geometry_msgs
)

install(TARGETS
  local_map_publisher
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
