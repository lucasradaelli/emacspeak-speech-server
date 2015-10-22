cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)

# 32-bit project, in order to dynamically link with libibmeci.so.
set(CMAKE_TOOLCHAIN_FILE "cmake/multilib-i386-linux-gnu.cmake")
project(emacspeak-speech-server CXX)

# Enable the C++11 standard in the compiler.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

# Libraries.
find_package(ALSA REQUIRED)

# Use Boost.Regex since the libstdc++-4.8 version that comes with
# Ubuntu 14.04 LTS has a broken implementation of C++11's <regex>.
find_package(Boost 1.54.0 REQUIRED COMPONENTS regex)
include_directories(${Boost_INCLUDE_DIRS})

# Path to the IBM ViaVoice Text-to-Speech SDK include file (eci.h).
include_directories("${PROJECT_SOURCE_DIR}/third_party/ibmtts-sdk")

# The speech server executable.
add_executable(speech_server
    server_main.cc
    alsa_player.cc alsa_player.h
    audio_manager.cc audio_manager.h
    audio_tasks.cc audio_tasks.h
    command_generator.cc command_generator.h
    commands.cc commands.h
    eci-c++.cc eci-c++.h
    server_state.cc server_state.h
    speech_server.cc speech_server.h
    tts.cc tts.h
)
target_link_libraries(speech_server
    ${ALSA_LIBRARY}
    ${Boost_REGEX_LIBRARIES}
    ${CMAKE_DL_LIBS}
)
