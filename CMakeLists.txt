# 指定使用 CMake 的最低版本号
cmake_minimum_required(VERSION 3.0)

# 设置项目名称
project(mytinymuduo C CXX)

# cmake => makefile   make
# mymuduo最终编译成so动态库，设置动态库的路径，放在根目录的lib文件夹下面
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# 设置调试信息 以及 启动C++11语言标准
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -fPIC")

# 设置项目编译头文件搜索路径 -I
include_directories(${PROJECT_SOURCE_DIR}/src/base)
include_directories(${PROJECT_SOURCE_DIR}/src/http)
include_directories(${PROJECT_SOURCE_DIR}/src/muduo_core)
include_directories(${PROJECT_SOURCE_DIR}/src/timer)
include_directories(${PROJECT_SOURCE_DIR}/src/logger)
include_directories(${PROJECT_SOURCE_DIR}/src/keepalive)
# include_directories(${PROJECT_SOURCE_DIR}/src/mysql)

# 设置项目库文件搜索路径 -L
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 定义参与编译的源代码文件 
aux_source_directory(${PROJECT_SOURCE_DIR}/src/base/ SRC_BASE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/http/ SRC_HTTP)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/timer/ SRC_TIMER)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/logger/ SRC_LOG)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/muduo_core/ SRC_MUDUO_CORE)
aux_source_directory(${PROJECT_SOURCE_DIR}/src/keepalive/ SRC_KEEPALIVE)
# aux_source_directory(${PROJECT_SOURCE_DIR}/src/mysql SRC_MYSQL)


# 编译生成动态库mytinymuduo
add_library(mytinymuduo SHARED 
            ${SRC_BASE}
            ${SRC_TIMER}
            ${SRC_LOG}
            ${SRC_HTTP}
            ${SRC_MUDUO_CORE}
            ${SRC_MYSQL}
            )

# 目标动态库所需连接的库 libthread libmysqlclient
target_link_libraries(mytinymuduo pthread)

# 设置安装的默认路径
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

#  将src的头文件中的.h文件复制到lib/include/mytinymuduo 下
install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/ DESTINATION lib/include/mytinymuduo
        FILES_MATCHING PATTERN "*.h")


add_subdirectory(src/http)

add_subdirectory(example/server)

add_subdirectory(example/server2)

add_subdirectory(example/client)