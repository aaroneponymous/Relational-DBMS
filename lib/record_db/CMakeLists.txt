cmake_minimum_required(VERSION 3.22.1)
project(RecordDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME}  record_db.h record_db.cpp record_db_example.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)