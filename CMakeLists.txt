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
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS
    "-Wall -Werror -lpthread -g"
)
add_executable (RstpServer "src/live/RtspServer.cpp" "src/main.cpp" "src/helper/SocketHelper.cpp" "src/live/RtspConnection.cpp" "src/live/Rtp.cpp" "src/live/H264MediaSource.cpp" "src/helper/RingBuffer.cpp" "src/helper/ThreadPool.cpp" "src/helper/Scheduler.cpp" "src/helper/Poller.cpp" "src/helper/Event.cpp" "src/live/MediaSession.cpp" "include/live/MediaSessionManager.h" "src/live/MediaSessionManager.cpp" "include/live/AACMediaSink.h" "include/live/AACMediaSource.h" "src/live/AACMediaSource.cpp" "src/live/AACMediaSink.cpp")
target_link_libraries(RstpServer pthread)
add_compile_definitions(ROOT_DIR="${PROJECT_SOURCE_DIR}")
# 将源代码添加到此项目的可执行文件。
add_executable (UDP_test "test/UDP.cpp")



# TODO: 如有需要，请添加测试并安装目标。



