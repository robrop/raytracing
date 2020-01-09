cmake_minimum_required(VERSION 3.9)

project(ray)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

#file(GLOB_RECURSE file_list "${CMAKE_CURRENT_LIST_DIR}/*.cpp" )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
