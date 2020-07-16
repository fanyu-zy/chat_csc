# CMakeList.txt: chat_socket 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
project(chat_csc)

# 将源代码添加到此项目的可执行文件。

include_directories("${PROJECT_BINARY_DIR}")

# Find all main*.cpp files and store in list mains
file(GLOB_RECURSE mains RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/chat*.cpp")
foreach(mainfile IN LISTS mains)
    # Get file name without directory
    get_filename_component(mainname ${mainfile} NAME_WE)
    add_executable(${mainname} ${mainfile})
endforeach()
# TODO: 如有需要，请添加测试并安装目标。

# TODO: 如有需要，请添加测试并安装目标。
