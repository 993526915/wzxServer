
# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -g -pthread")
PROJECT(wzxServer)

ADD_SUBDIRECTORY(src bin)

MESSAGE(STATUS "TOP CMAKE_CURRENT_SOURCE_DIR -> " ${CMAKE_CURRENT_SOURCE_DIR})

#INSTALL(FILES COPYRIGHT README DESTINATION share/doc/cmake/0voice)
#INSTALL(DIRECTORY doc/ DESTINATION share/doc/cmake/0voice)
