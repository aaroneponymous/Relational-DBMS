# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build"

# Include any dependencies generated for this target.
include CMakeFiles/RecordDBExample.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/RecordDBExample.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/RecordDBExample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/RecordDBExample.dir/flags.make

CMakeFiles/RecordDBExample.dir/record_db.cpp.o: CMakeFiles/RecordDBExample.dir/flags.make
CMakeFiles/RecordDBExample.dir/record_db.cpp.o: ../record_db.cpp
CMakeFiles/RecordDBExample.dir/record_db.cpp.o: CMakeFiles/RecordDBExample.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/RecordDBExample.dir/record_db.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/RecordDBExample.dir/record_db.cpp.o -MF CMakeFiles/RecordDBExample.dir/record_db.cpp.o.d -o CMakeFiles/RecordDBExample.dir/record_db.cpp.o -c "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/record_db.cpp"

CMakeFiles/RecordDBExample.dir/record_db.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/RecordDBExample.dir/record_db.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/record_db.cpp" > CMakeFiles/RecordDBExample.dir/record_db.cpp.i

CMakeFiles/RecordDBExample.dir/record_db.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/RecordDBExample.dir/record_db.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/record_db.cpp" -o CMakeFiles/RecordDBExample.dir/record_db.cpp.s

# Object files for target RecordDBExample
RecordDBExample_OBJECTS = \
"CMakeFiles/RecordDBExample.dir/record_db.cpp.o"

# External object files for target RecordDBExample
RecordDBExample_EXTERNAL_OBJECTS =

RecordDBExample: CMakeFiles/RecordDBExample.dir/record_db.cpp.o
RecordDBExample: CMakeFiles/RecordDBExample.dir/build.make
RecordDBExample: CMakeFiles/RecordDBExample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable RecordDBExample"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RecordDBExample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/RecordDBExample.dir/build: RecordDBExample
.PHONY : CMakeFiles/RecordDBExample.dir/build

CMakeFiles/RecordDBExample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/RecordDBExample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/RecordDBExample.dir/clean

CMakeFiles/RecordDBExample.dir/depend:
	cd "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db" "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db" "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build" "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build" "/home/aaroneponymous/Aaron's Folder/University/Semesters/spring_2024/csc_263/project_1/lib/record_db/build/CMakeFiles/RecordDBExample.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/RecordDBExample.dir/depend

