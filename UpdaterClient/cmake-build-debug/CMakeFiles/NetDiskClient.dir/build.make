# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.19

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Software\Jetbrains\CLion 2021.1.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Software\Jetbrains\CLion 2021.1.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Scott\Code\Updater\UpdaterClient

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/NetDiskClient.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/NetDiskClient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/NetDiskClient.dir/flags.make

CMakeFiles/NetDiskClient.dir/src/cJSON.c.obj: CMakeFiles/NetDiskClient.dir/flags.make
CMakeFiles/NetDiskClient.dir/src/cJSON.c.obj: ../src/cJSON.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/NetDiskClient.dir/src/cJSON.c.obj"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\NetDiskClient.dir\src\cJSON.c.obj -c C:\Scott\Code\Updater\UpdaterClient\src\cJSON.c

CMakeFiles/NetDiskClient.dir/src/cJSON.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/NetDiskClient.dir/src/cJSON.c.i"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Scott\Code\Updater\UpdaterClient\src\cJSON.c > CMakeFiles\NetDiskClient.dir\src\cJSON.c.i

CMakeFiles/NetDiskClient.dir/src/cJSON.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/NetDiskClient.dir/src/cJSON.c.s"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Scott\Code\Updater\UpdaterClient\src\cJSON.c -o CMakeFiles\NetDiskClient.dir\src\cJSON.c.s

CMakeFiles/NetDiskClient.dir/src/Main.c.obj: CMakeFiles/NetDiskClient.dir/flags.make
CMakeFiles/NetDiskClient.dir/src/Main.c.obj: ../src/Main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/NetDiskClient.dir/src/Main.c.obj"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles\NetDiskClient.dir\src\Main.c.obj -c C:\Scott\Code\Updater\UpdaterClient\src\Main.c

CMakeFiles/NetDiskClient.dir/src/Main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/NetDiskClient.dir/src/Main.c.i"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E C:\Scott\Code\Updater\UpdaterClient\src\Main.c > CMakeFiles\NetDiskClient.dir\src\Main.c.i

CMakeFiles/NetDiskClient.dir/src/Main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/NetDiskClient.dir/src/Main.c.s"
	C:\Software\mingw64\bin\gcc.exe $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S C:\Scott\Code\Updater\UpdaterClient\src\Main.c -o CMakeFiles\NetDiskClient.dir\src\Main.c.s

# Object files for target NetDiskClient
NetDiskClient_OBJECTS = \
"CMakeFiles/NetDiskClient.dir/src/cJSON.c.obj" \
"CMakeFiles/NetDiskClient.dir/src/Main.c.obj"

# External object files for target NetDiskClient
NetDiskClient_EXTERNAL_OBJECTS =

build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/src/cJSON.c.obj
build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/src/Main.c.obj
build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/build.make
build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/linklibs.rsp
build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/objects1.rsp
build/NetDiskClient.exe: CMakeFiles/NetDiskClient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable build\NetDiskClient.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\NetDiskClient.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/NetDiskClient.dir/build: build/NetDiskClient.exe

.PHONY : CMakeFiles/NetDiskClient.dir/build

CMakeFiles/NetDiskClient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\NetDiskClient.dir\cmake_clean.cmake
.PHONY : CMakeFiles/NetDiskClient.dir/clean

CMakeFiles/NetDiskClient.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Scott\Code\Updater\UpdaterClient C:\Scott\Code\Updater\UpdaterClient C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug C:\Scott\Code\Updater\UpdaterClient\cmake-build-debug\CMakeFiles\NetDiskClient.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/NetDiskClient.dir/depend

