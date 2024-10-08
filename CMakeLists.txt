cmake_minimum_required(VERSION 3.22.1)
project(PageDB)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/lib/page_layout_db)
include_directories(${PROJECT_SOURCE_DIR}/lib/record_db)
include_directories(${PROJECT_SOURCE_DIR}/lib/heap_file_db)

# Set the output directory for executables to 'exe/' in the current working directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/exe)

# Add executables
add_executable(write_fixed_len_pages lib/page_layout_db/write_fixed_len_pages.cpp 
                                     lib/page_layout_db/page_layout_db.cpp
                                     lib/page_layout_db/page_layout_db.h
                                     lib/record_db/record_db.h
                                     lib/record_db/record_db.cpp
                                     )
add_executable(read_fixed_len_pages lib/page_layout_db/read_fixed_len_pages.cpp 
                                    lib/page_layout_db/page_layout_db.h
                                    lib/page_layout_db/page_layout_db.cpp
                                    lib/record_db/record_db.h
                                    lib/record_db/record_db.cpp)

# Add a custom target to clean executables
# Add a custom target to clean executables
add_custom_target(clean_exe
    COMMAND ${CMAKE_COMMAND} -E remove ../exe/write_fixed_len_pages
    COMMAND ${CMAKE_COMMAND} -E remove ../exe/read_fixed_len_pages
    COMMENT "Cleaning executables"
)
