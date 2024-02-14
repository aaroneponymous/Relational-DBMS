# Data-Management-Sys-Project-1
#

$$
\large\text{Project Structure}
$$


Root directory of the project, “project_1/” consists of the following:

- subdirectories:
    - build/
        - files built by cmake containing the executable to run the project
        - Source CMakeLists.txt path from pwd: ../CMakeLists.txt
    - lib/
        - contains the header, cpp (implementation) and example files (to test individual implementation) of the library data structures and functions within respective modules
            - page_layout_db/
                - *namespace* encapsulating the latter defined as *dbms::page*
                    - page_layout_db.h
                    - page_layout_db.cpp
                    - page_layout_example.cpp
            
            - record_db/
                - Implements the serialization and deserialization of fixed-length and variable-length records
                - *namespace* encapsulating the latter defined as *dbms::record_codec*
                    - record_db.h
                    - record_db.cpp
                    - record_dp_example.cpp
    - src/
    - files in top-level directory
        - CMakeLists.txt
        - README.md