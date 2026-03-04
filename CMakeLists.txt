cmake_minimum_required(VERSION 3.10)

project(umbra_re VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE UMBRA_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")
if(NOT UMBRA_SOURCES)
	list(APPEND UMBRA_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
endif()

add_executable(umbra_re ${UMBRA_SOURCES})

target_include_directories(umbra_re PRIVATE ${CMAKE_SOURCE_DIR}/include)

if(MSVC)
	target_compile_options(umbra_re PRIVATE /W4 /permissive-)
else()
	target_compile_options(umbra_re PRIVATE -g -Wall -Wextra -Wpedantic)
endif()

option(UMBRA_BENCHMARK "Habilitar benchmarks en tiempo de ejecucion" OFF)
if(UMBRA_BENCHMARK)
	target_compile_definitions(umbra_re PRIVATE UMBRA_BENCHMARK)
	message(STATUS "Benchmarks: HABILITADOS")
else()
	message(STATUS "Benchmarks: deshabilitados (usar -DUMBRA_BENCHMARK=ON para activar)")
endif()

message(STATUS "Project: ${PROJECT_NAME} — Sources: ${UMBRA_SOURCES}")

