cmake_minimum_required(VERSION 3.22.1)
project(PageDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/lib/page_layout_db)
include_directories(${PROJECT_SOURCE_DIR}/lib/record_db)

# Add executable
add_executable(PageLayoutExample lib/page_layout_db/page_layout_example.cpp lib/page_layout_db/page_layout_db.cpp lib/record_db/record_db.cpp)

