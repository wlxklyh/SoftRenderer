cmake_minimum_required(VERSION 3.7)
# 指定C++11
set(CMAKE_CXX_STANDARD 11)
MESSAGE(STATUS "Main cmake code")
set(NAME SoftRenderer)
project(SoftRenderer)

add_executable(SoftRenderer Main.cpp)

include_directories(ThirdParty/easyx/include)

target_link_libraries(${PROJECT_NAME}
        EasyXa
        EasyXw
        )

target_link_directories(${PROJECT_NAME} PUBLIC  ${CMAKE_SOURCE_DIR}/ThirdParty/easyx/lib/)