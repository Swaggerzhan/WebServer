# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /tmp/tmp.lGJb5QMYLR

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /tmp/tmp.lGJb5QMYLR/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/WebServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/WebServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WebServer.dir/flags.make

CMakeFiles/WebServer.dir/main.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.lGJb5QMYLR/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/WebServer.dir/main.cpp.o"
	/usr/bin/g++-7   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebServer.dir/main.cpp.o -c /tmp/tmp.lGJb5QMYLR/main.cpp

CMakeFiles/WebServer.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/main.cpp.i"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.lGJb5QMYLR/main.cpp > CMakeFiles/WebServer.dir/main.cpp.i

CMakeFiles/WebServer.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/main.cpp.s"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.lGJb5QMYLR/main.cpp -o CMakeFiles/WebServer.dir/main.cpp.s

CMakeFiles/WebServer.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/WebServer.dir/main.cpp.o.requires

CMakeFiles/WebServer.dir/main.cpp.o.provides: CMakeFiles/WebServer.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/WebServer.dir/build.make CMakeFiles/WebServer.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/WebServer.dir/main.cpp.o.provides

CMakeFiles/WebServer.dir/main.cpp.o.provides.build: CMakeFiles/WebServer.dir/main.cpp.o


CMakeFiles/WebServer.dir/TEST/DI.cpp.o: CMakeFiles/WebServer.dir/flags.make
CMakeFiles/WebServer.dir/TEST/DI.cpp.o: ../TEST/DI.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/tmp/tmp.lGJb5QMYLR/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/WebServer.dir/TEST/DI.cpp.o"
	/usr/bin/g++-7   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/WebServer.dir/TEST/DI.cpp.o -c /tmp/tmp.lGJb5QMYLR/TEST/DI.cpp

CMakeFiles/WebServer.dir/TEST/DI.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/WebServer.dir/TEST/DI.cpp.i"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /tmp/tmp.lGJb5QMYLR/TEST/DI.cpp > CMakeFiles/WebServer.dir/TEST/DI.cpp.i

CMakeFiles/WebServer.dir/TEST/DI.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/WebServer.dir/TEST/DI.cpp.s"
	/usr/bin/g++-7  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /tmp/tmp.lGJb5QMYLR/TEST/DI.cpp -o CMakeFiles/WebServer.dir/TEST/DI.cpp.s

CMakeFiles/WebServer.dir/TEST/DI.cpp.o.requires:

.PHONY : CMakeFiles/WebServer.dir/TEST/DI.cpp.o.requires

CMakeFiles/WebServer.dir/TEST/DI.cpp.o.provides: CMakeFiles/WebServer.dir/TEST/DI.cpp.o.requires
	$(MAKE) -f CMakeFiles/WebServer.dir/build.make CMakeFiles/WebServer.dir/TEST/DI.cpp.o.provides.build
.PHONY : CMakeFiles/WebServer.dir/TEST/DI.cpp.o.provides

CMakeFiles/WebServer.dir/TEST/DI.cpp.o.provides.build: CMakeFiles/WebServer.dir/TEST/DI.cpp.o


# Object files for target WebServer
WebServer_OBJECTS = \
"CMakeFiles/WebServer.dir/main.cpp.o" \
"CMakeFiles/WebServer.dir/TEST/DI.cpp.o"

# External object files for target WebServer
WebServer_EXTERNAL_OBJECTS =

WebServer: CMakeFiles/WebServer.dir/main.cpp.o
WebServer: CMakeFiles/WebServer.dir/TEST/DI.cpp.o
WebServer: CMakeFiles/WebServer.dir/build.make
WebServer: CMakeFiles/WebServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/tmp/tmp.lGJb5QMYLR/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable WebServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WebServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WebServer.dir/build: WebServer

.PHONY : CMakeFiles/WebServer.dir/build

CMakeFiles/WebServer.dir/requires: CMakeFiles/WebServer.dir/main.cpp.o.requires
CMakeFiles/WebServer.dir/requires: CMakeFiles/WebServer.dir/TEST/DI.cpp.o.requires

.PHONY : CMakeFiles/WebServer.dir/requires

CMakeFiles/WebServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WebServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WebServer.dir/clean

CMakeFiles/WebServer.dir/depend:
	cd /tmp/tmp.lGJb5QMYLR/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /tmp/tmp.lGJb5QMYLR /tmp/tmp.lGJb5QMYLR /tmp/tmp.lGJb5QMYLR/cmake-build-debug /tmp/tmp.lGJb5QMYLR/cmake-build-debug /tmp/tmp.lGJb5QMYLR/cmake-build-debug/CMakeFiles/WebServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WebServer.dir/depend

