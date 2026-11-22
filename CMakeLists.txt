cmake_minimum_required(VERSION 3.10)
project(ChessGame CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
set(LIB_SOURCES ${SOURCES})
list(FILTER LIB_SOURCES EXCLUDE REGEX ".*/main\\.cpp$")

add_library(chesslib STATIC ${LIB_SOURCES})
target_include_directories(chesslib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(chess src/main.cpp)
target_link_libraries(chess PRIVATE chesslib)

add_executable(test_chess tests/test_chess.cpp)
target_link_libraries(test_chess PRIVATE chesslib)

enable_testing()
add_test(NAME chess_tests COMMAND test_chess)

if(MSVC)
  target_compile_options(chesslib PRIVATE /W4)
else()
  target_compile_options(chesslib PRIVATE -Wall -Wextra -Wpedantic)
endif()
