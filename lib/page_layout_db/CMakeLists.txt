cmake_minimum_required(VERSION 3.22.1)
project(PageDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} page_layout_db.h page_layout_db.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)