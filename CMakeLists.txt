set(PROJECT qtawss3)
project(${PROJECT} C CXX)
cmake_minimum_required(VERSION 2.8.11)

option(TEST_ENABLE "Enable tests" OFF)
option(TEST_DATASET "Enable test dataset" OFF)

set (Qt5_COMMPONETS Core
                    Widgets
                    Network
                    Concurrent)
                    
find_package(Qt5 COMPONENTS ${Qt5_COMMPONETS})

include_directories(${Qt5Core_INCLUDE_DIRS}
                    ${Qt5Widgets_INCLUDE_DIRS}
                    ${Qt5Network_INCLUDE_DIRS}
                    ${Qt5Concurrent_INCLUDE_DIRS})
                    
add_definitions(${Qt5Core_DEFINITIONS}
                ${Qt5Widgets_DEFINITIONS}
                ${Qt5Network_DEFINITIONS}
                ${Qt5Concurrent_DEFINITIONS})
                
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra -Wpedantic")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(TEST_DATASET)
    message("test data set")
    add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/test/data")
    add_definitions(-DTEST_DATASET)
endif()

add_subdirectory(src)

include_directories(${CMAKE_SOURCE_DIR}/src/core
                    ${CMAKE_SOURCE_DIR}/src/ui
)

if(TEST_ENABLE)
    add_definitions(-DDATA_DIR="${CMAKE_SOURCE_DIR}/test/data")
    enable_testing()
    add_subdirectory(3rdparty/gtest-1.7.0)
    include_directories(3rdparty/gtest-1.7.0/include
    )
    add_subdirectory(test)
endif()
