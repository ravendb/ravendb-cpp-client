# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /opt/clion-2019.1.4/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2019.1.4/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alexander/RavenDB_Client/raven-cpp-client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug

# Include any dependencies generated for this target.
include Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/depend.make

# Include the progress variables for this target.
include Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/progress.make

# Include the compile flags for this target's objects.
include Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/flags.make

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/flags.make
Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o: ../Tryouts/pch.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o -c /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/pch.cpp

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.i"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/pch.cpp > CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.i

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.s"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/pch.cpp -o CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.s

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/flags.make
Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o: ../Tryouts/Tryouts.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o -c /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/Tryouts.cpp

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.i"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/Tryouts.cpp > CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.i

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.s"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts/Tryouts.cpp -o CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.s

# Object files for target Raven_CppClient_Tryouts
Raven_CppClient_Tryouts_OBJECTS = \
"CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o" \
"CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o"

# External object files for target Raven_CppClient_Tryouts
Raven_CppClient_Tryouts_EXTERNAL_OBJECTS =

Tryouts/Raven_CppClient_Tryouts: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/pch.cpp.o
Tryouts/Raven_CppClient_Tryouts: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/Tryouts.cpp.o
Tryouts/Raven_CppClient_Tryouts: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/build.make
Tryouts/Raven_CppClient_Tryouts: Raven.CppClient/libRaven_CppClient.so
Tryouts/Raven_CppClient_Tryouts: Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable Raven_CppClient_Tryouts"
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Raven_CppClient_Tryouts.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/build: Tryouts/Raven_CppClient_Tryouts

.PHONY : Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/build

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/clean:
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts && $(CMAKE_COMMAND) -P CMakeFiles/Raven_CppClient_Tryouts.dir/cmake_clean.cmake
.PHONY : Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/clean

Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/depend:
	cd /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alexander/RavenDB_Client/raven-cpp-client /home/alexander/RavenDB_Client/raven-cpp-client/Tryouts /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts /home/alexander/RavenDB_Client/raven-cpp-client/cmake-build-debug/Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Tryouts/CMakeFiles/Raven_CppClient_Tryouts.dir/depend

