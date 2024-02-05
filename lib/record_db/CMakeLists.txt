cmake_minimum_required(VERSION 3.22.1)
project(RecordDBExample)

set(CMAKE_CXX_STANDARD 17) # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(RecordDBExample record_db.h record_db.cpp)

