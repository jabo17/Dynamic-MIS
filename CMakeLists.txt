cmake_minimum_required(VERSION 3.16)

project(Dynamic-MIS)

include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_RELEASE} -O3")

include_directories(src/)

add_executable(dyoneswap src/DyOneSwap.cpp)
add_executable(dytwoswap src/DyTwoSwap.cpp)
add_executable(lazydyoneswap src/LazyDyOneSwap.cpp)
add_executable(lazydytwoswap src/LazyDyTwoSwap.cpp)

set(CMAKE_INSTALL_PREFIX ../deploy)
install(TARGETS dyoneswap dytwoswap lazydyoneswap lazydytwoswap DESTINATION bin)
