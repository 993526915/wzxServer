# 单个目录实现
# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.0)
# Set cflags
#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -g -fno-strict-aliasing -O2 -Wall -export-dynamic -Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64)
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -g -pthread")
# Use c and asm
enable_language(C ASM)

# 工程
PROJECT(wzxServer)
# 手动加入文件
find_package(OpenCV REQUIRED)
find_package(OpenSSL REQUIRED)

find_package(jsoncpp CONFIG REQUIRED)
SET(SRC_LIST main.cpp coctx_swap.S params.json myserver.cpp myserver.h)
MESSAGE(STATUS "THIS IS BINARY DIR " ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "THIS IS SOURCE DIR " ${PROJECT_SOURCE_DIR})
set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")
include_directories(${OpenCV_INCLUDE_DIRS})
#设置子目录
set(SUB_DIR_LIST "${CMAKE_CURRENT_SOURCE_DIR}/env"
                 "${CMAKE_CURRENT_SOURCE_DIR}/socket"
                 "${CMAKE_CURRENT_SOURCE_DIR}/coroutine"
                 "${CMAKE_CURRENT_SOURCE_DIR}/detectmodel"
                 "${CMAKE_CURRENT_SOURCE_DIR}/params"
                 "${CMAKE_CURRENT_SOURCE_DIR}/dbpool"
                 "${CMAKE_CURRENT_SOURCE_DIR}/relationmodel"
                  "${CMAKE_CURRENT_SOURCE_DIR}/sessionmodel"
                 "${CMAKE_CURRENT_SOURCE_DIR}/usermodel"
                 "${CMAKE_CURRENT_SOURCE_DIR}/messagemodel"
                 )

foreach(SUB_DIR ${SUB_DIR_LIST})
    #遍历源文件
    aux_source_directory(${SUB_DIR} SRC_LIST)
    MESSAGE(STATUS "SUB_DIR-> " ${SUB_DIR})
    MESSAGE(STATUS "SRC_LIST-> " ${SRC_LIST})
endforeach()

# 添加头文件路径

INCLUDE_DIRECTORIES("socket")
INCLUDE_DIRECTORIES("coroutine")
INCLUDE_DIRECTORIES("env")
INCLUDE_DIRECTORIES("detectmodel")
INCLUDE_DIRECTORIES("params")
INCLUDE_DIRECTORIES("dbpool")
INCLUDE_DIRECTORIES("relationmodel")
INCLUDE_DIRECTORIES("sessionmodel")
INCLUDE_DIRECTORIES("usermodel")
INCLUDE_DIRECTORIES("messagemodel")
INCLUDE_DIRECTORIES("linux_so")
MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})
link_directories("${CMAKE_CURRENT_SOURCE_DIR}/linux_so")
ADD_EXECUTABLE(wzxServer ${SRC_LIST} )
target_link_libraries(${PROJECT_NAME} dl ${OpenCV_LIBS} mysqlclient jsoncpp_lib crypto curl arcsoft_face arcsoft_face_engine)
#target_link_libraries(wzxServer ${OpenCV_LIBS})
##include_directories(${JSON_INC_PATH})#通过这个来添加库
#target_link_libraries(${PROJECT_NAME} jsoncpp_lib)
# 将执行文件安装到bin目录
#INSTALL(TARGETS wzxServer RUNTIME DESTINATION bin)
