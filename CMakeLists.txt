# CMakeList.txt: RstpServer 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.12)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (RstpServer)
include_directories(${PROJECT_SOURCE_DIR}/include/)

set(CMAKE_CXX_FLAGS
    "-std=c++17 -Wall -Werror -lpthread"
)
add_executable (RstpServer "src/RtspServer.cpp" "src/main.cpp" "src/SocketHelper.cpp" "src/RtspConnection.cpp" "include/RtspConnection.h" "include/Rtp.h" "src/Rtp.cpp" "include/H264MediaSource.h" "src/H264MediaSource.cpp" "include/Frame.h")
target_link_libraries(RstpServer pthread)
add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}")
# 将源代码添加到此项目的可执行文件。


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RstpServer PROPERTY CXX_STANDARD 17)
endif()

# TODO: 如有需要，请添加测试并安装目标。
