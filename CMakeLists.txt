cmake_minimum_required(VERSION 3.0)
project(mychat)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/include)#头文件路径
link_directories(${CMAKE_SOURCE_DIR}/lib)#库文件路径

# set(LIST01 
#     ./src/log.cpp
#     ./examples/01.日志器测试.cpp
#     )

# set(LIST02
#     ./src/log.cpp
#     ./examples/02.日志器测试.cpp
# )

# set(LIST03
#     ./src/log.cpp
#     ./examples/03.日志器测试.cpp
# )

# set(LIST04
#     ./src/log.cpp
#     ./examples/04.日志器测试.cpp
# )

# set(LIST05
#     ./src/log.cpp
#     ./examples/05.日志器测试.cpp
# )

# set(LIST06
#     ./src/log.cpp
#     ./examples/06.日志器测试.cpp
# )

set(LIST07
    ./src/log.cpp
    ./examples/07.日志器测试.cpp
)

# add_executable(01test_logger ${LIST01})
# add_executable(02test_logger ${LIST02})
#  add_executable(03test_logger ${LIST03})
# add_executable(04test_logger ${LIST04})
# add_executable(05test_logger ${LIST05})
# add_executable(06test_logger ${LIST06})
add_executable(07test_logger ${LIST07})

#target_link_libraries(01.日志器测试  pthread )

