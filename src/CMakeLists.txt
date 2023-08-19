# 定义变量SRC_LIST代表当前文件夹下所有源文件
aux_source_directory(./ SRC_LIST)

# 指定生成可执行文件
add_executable(connection ${SRC_LIST})

# 指定所需要连接的库
target_link_libraries(connection libmysqlclient.so pthread)