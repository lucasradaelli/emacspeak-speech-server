# CMake toolchain file to enable multilib 32-bit compilation.
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR i686)
SET(CMAKE_C_COMPILER   /usr/bin/gcc -m32)
SET(CMAKE_CXX_COMPILER /usr/bin/g++ -m32)
