cmake_minimum_required(VERSION 3.11.3)

# set(CMAKE_CXX_STANDARD 17)

#  ========================================================================================
#  CMake Error at CMakeLists.txt:14 (find_package):
#   By not providing "FindOpenCV.cmake" in CMAKE_MODULE_PATH this project has
#   asked CMake to find a package configuration file provided by "OpenCV", but
#   CMake did not find one.
# 
#   Could not find a package configuration file provided by "OpenCV" (requested
#   version 4.1) with any of the following names:
# 
#     OpenCVConfig.cmake
#     opencv-config.cmake
# 
#   Add the installation prefix of "OpenCV" to CMAKE_PREFIX_PATH or set
#   "OpenCV_DIR" to a directory containing one of the above files.  If "OpenCV"
#   provides a separate development package or SDK, be sure it has been
#   installed.
# 
#  ========================================================================================
## FAIL - set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /home/xavi/opencv_build/opencv/build)
set(OpenCV_DIR "/home/xavi/opencv_build/opencv/build")

## list(APPEND CMAKE_MODULE_PATH "/home/xavi/opencv_build/opencv/build")
## OpenCVConfig.cmake
# ========================================================================================

project(traffic_simulation)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

find_package(OpenCV 4.1 REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARY_DIRS})
add_definitions(${OpenCV_DEFINITIONS})

# Find all executables
file(GLOB project_SRCS src/*.cpp) #src/*.h

# Add project executable
add_executable(traffic_simulation ${project_SRCS})
target_link_libraries(traffic_simulation ${OpenCV_LIBRARIES})
