# 指定cmake最低版本
cmake_minimum_required(VERSION 3.0)

# 设置项目名称
project(MysqlConnectionPool)

# 设置编译std=c++11
set(CMAKE_CXX_STANDARD 11)

# 配置编译选项
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} -g)

# 指定可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 指定头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 加载子目录
add_subdirectory(src)




