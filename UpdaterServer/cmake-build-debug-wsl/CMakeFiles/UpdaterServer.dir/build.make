# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /mnt/c/Scott/Code/Updater/UpdaterServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl

# Include any dependencies generated for this target.
include CMakeFiles/UpdaterServer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/UpdaterServer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/UpdaterServer.dir/flags.make

CMakeFiles/UpdaterServer.dir/src/cJSON.c.o: CMakeFiles/UpdaterServer.dir/flags.make
CMakeFiles/UpdaterServer.dir/src/cJSON.c.o: ../src/cJSON.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/UpdaterServer.dir/src/cJSON.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/UpdaterServer.dir/src/cJSON.c.o   -c /mnt/c/Scott/Code/Updater/UpdaterServer/src/cJSON.c

CMakeFiles/UpdaterServer.dir/src/cJSON.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/UpdaterServer.dir/src/cJSON.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Scott/Code/Updater/UpdaterServer/src/cJSON.c > CMakeFiles/UpdaterServer.dir/src/cJSON.c.i

CMakeFiles/UpdaterServer.dir/src/cJSON.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/UpdaterServer.dir/src/cJSON.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Scott/Code/Updater/UpdaterServer/src/cJSON.c -o CMakeFiles/UpdaterServer.dir/src/cJSON.c.s

CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o: CMakeFiles/UpdaterServer.dir/flags.make
CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o: ../src/FileSystem.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o   -c /mnt/c/Scott/Code/Updater/UpdaterServer/src/FileSystem.c

CMakeFiles/UpdaterServer.dir/src/FileSystem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/UpdaterServer.dir/src/FileSystem.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Scott/Code/Updater/UpdaterServer/src/FileSystem.c > CMakeFiles/UpdaterServer.dir/src/FileSystem.c.i

CMakeFiles/UpdaterServer.dir/src/FileSystem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/UpdaterServer.dir/src/FileSystem.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Scott/Code/Updater/UpdaterServer/src/FileSystem.c -o CMakeFiles/UpdaterServer.dir/src/FileSystem.c.s

CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o: CMakeFiles/UpdaterServer.dir/flags.make
CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o: ../src/NetworkSystem.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o   -c /mnt/c/Scott/Code/Updater/UpdaterServer/src/NetworkSystem.c

CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Scott/Code/Updater/UpdaterServer/src/NetworkSystem.c > CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.i

CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Scott/Code/Updater/UpdaterServer/src/NetworkSystem.c -o CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.s

CMakeFiles/UpdaterServer.dir/src/Main.c.o: CMakeFiles/UpdaterServer.dir/flags.make
CMakeFiles/UpdaterServer.dir/src/Main.c.o: ../src/Main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/UpdaterServer.dir/src/Main.c.o"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/UpdaterServer.dir/src/Main.c.o   -c /mnt/c/Scott/Code/Updater/UpdaterServer/src/Main.c

CMakeFiles/UpdaterServer.dir/src/Main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/UpdaterServer.dir/src/Main.c.i"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/c/Scott/Code/Updater/UpdaterServer/src/Main.c > CMakeFiles/UpdaterServer.dir/src/Main.c.i

CMakeFiles/UpdaterServer.dir/src/Main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/UpdaterServer.dir/src/Main.c.s"
	/usr/bin/gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/c/Scott/Code/Updater/UpdaterServer/src/Main.c -o CMakeFiles/UpdaterServer.dir/src/Main.c.s

# Object files for target UpdaterServer
UpdaterServer_OBJECTS = \
"CMakeFiles/UpdaterServer.dir/src/cJSON.c.o" \
"CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o" \
"CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o" \
"CMakeFiles/UpdaterServer.dir/src/Main.c.o"

# External object files for target UpdaterServer
UpdaterServer_EXTERNAL_OBJECTS =

build/UpdaterServer: CMakeFiles/UpdaterServer.dir/src/cJSON.c.o
build/UpdaterServer: CMakeFiles/UpdaterServer.dir/src/FileSystem.c.o
build/UpdaterServer: CMakeFiles/UpdaterServer.dir/src/NetworkSystem.c.o
build/UpdaterServer: CMakeFiles/UpdaterServer.dir/src/Main.c.o
build/UpdaterServer: CMakeFiles/UpdaterServer.dir/build.make
build/UpdaterServer: CMakeFiles/UpdaterServer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable build/UpdaterServer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/UpdaterServer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/UpdaterServer.dir/build: build/UpdaterServer

.PHONY : CMakeFiles/UpdaterServer.dir/build

CMakeFiles/UpdaterServer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/UpdaterServer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/UpdaterServer.dir/clean

CMakeFiles/UpdaterServer.dir/depend:
	cd /mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/c/Scott/Code/Updater/UpdaterServer /mnt/c/Scott/Code/Updater/UpdaterServer /mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl /mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl /mnt/c/Scott/Code/Updater/UpdaterServer/cmake-build-debug-wsl/CMakeFiles/UpdaterServer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/UpdaterServer.dir/depend

