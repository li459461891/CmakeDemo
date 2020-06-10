# cmake version 
cmake_minimum_required(VERSION 3.0)
# cmake build
project(cmake-demo)

SET(CMAKE_BUILD_TYPE "Release")

# include directories for all configs
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x  -Wall -O3 -fpermissive -pthread")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fpermissive")

if(CMAKE_HOST_WIN32)
  if(CMAKE_CL_64)
    SET(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/win64)
  else()
    SET(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/win32)
    message("${CMAKE_CURRENT_SOURCE_DIR}/libs/win32")
  endif()
  link_directories(${LIB_DIR}/lib)
else(CMAKE_HOST_UNIX)
  SET(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/linux)
  link_directories(${LIB_DIR})
endif()

message("link directories : ${LIB_DIR}/lib")


set(source_files ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(cmake-demo ${source_files} )

target_link_libraries(cmake-demo zmq)

if(CMAKE_HOST_WIN32)
# copy dependencies (only shared libraries)
add_custom_command(TARGET cmake-demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${LIB_DIR}/bin/"
        $<TARGET_FILE_DIR:cmake-demo>)
endif()
