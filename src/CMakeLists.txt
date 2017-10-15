cmake_minimum_required(VERSION 2.6)

ADD_DEFINITIONS(-DREENTRANT)
include_directories (/usr/local/include)
link_directories(/usr/local/lib)

add_executable(videocapture main.cpp)

target_link_libraries (videocapture opencv_core opencv_highgui opencv_imgproc opencv_imgcodecs opencv_video opencv_videoio pthread)

install(TARGETS videocapture RUNTIME DESTINATION install)
