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


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /mnt/d/dev/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/dev/sylar/build

# Include any dependencies generated for this target.
include CMakeFiles/sylar.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/sylar.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/sylar.dir/flags.make

CMakeFiles/sylar.dir/sylar/config/config.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/config/config.cc.o: ../sylar/config/config.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/sylar.dir/sylar/config/config.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/config/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/config/config.cc.o -c /mnt/d/dev/sylar/sylar/config/config.cc

CMakeFiles/sylar.dir/sylar/config/config.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/config/config.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/config/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/config/config.cc > CMakeFiles/sylar.dir/sylar/config/config.cc.i

CMakeFiles/sylar.dir/sylar/config/config.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/config/config.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/config/config.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/config/config.cc -o CMakeFiles/sylar.dir/sylar/config/config.cc.s

CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o: ../sylar/fiber/fiber.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o -c /mnt/d/dev/sylar/sylar/fiber/fiber.cc

CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/fiber/fiber.cc > CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.i

CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/fiber.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/fiber/fiber.cc -o CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.s

CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o: ../sylar/fiber/iomanager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o -c /mnt/d/dev/sylar/sylar/fiber/iomanager.cc

CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/fiber/iomanager.cc > CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.i

CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/iomanager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/fiber/iomanager.cc -o CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.s

CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o: ../sylar/fiber/scheduler.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o -c /mnt/d/dev/sylar/sylar/fiber/scheduler.cc

CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/fiber/scheduler.cc > CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.i

CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/scheduler.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/fiber/scheduler.cc -o CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.s

CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o: ../sylar/fiber/timer.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/timer.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o -c /mnt/d/dev/sylar/sylar/fiber/timer.cc

CMakeFiles/sylar.dir/sylar/fiber/timer.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/fiber/timer.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/timer.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/fiber/timer.cc > CMakeFiles/sylar.dir/sylar/fiber/timer.cc.i

CMakeFiles/sylar.dir/sylar/fiber/timer.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/fiber/timer.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/fiber/timer.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/fiber/timer.cc -o CMakeFiles/sylar.dir/sylar/fiber/timer.cc.s

CMakeFiles/sylar.dir/sylar/log/log_level.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/log/log_level.cc.o: ../sylar/log/log_level.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/sylar.dir/sylar/log/log_level.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_level.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/log/log_level.cc.o -c /mnt/d/dev/sylar/sylar/log/log_level.cc

CMakeFiles/sylar.dir/sylar/log/log_level.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/log/log_level.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_level.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/log/log_level.cc > CMakeFiles/sylar.dir/sylar/log/log_level.cc.i

CMakeFiles/sylar.dir/sylar/log/log_level.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/log/log_level.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_level.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/log/log_level.cc -o CMakeFiles/sylar.dir/sylar/log/log_level.cc.s

CMakeFiles/sylar.dir/sylar/log/log_event.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/log/log_event.cc.o: ../sylar/log/log_event.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/sylar.dir/sylar/log/log_event.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_event.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/log/log_event.cc.o -c /mnt/d/dev/sylar/sylar/log/log_event.cc

CMakeFiles/sylar.dir/sylar/log/log_event.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/log/log_event.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_event.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/log/log_event.cc > CMakeFiles/sylar.dir/sylar/log/log_event.cc.i

CMakeFiles/sylar.dir/sylar/log/log_event.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/log/log_event.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_event.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/log/log_event.cc -o CMakeFiles/sylar.dir/sylar/log/log_event.cc.s

CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o: ../sylar/log/log_formatter.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_formatter.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o -c /mnt/d/dev/sylar/sylar/log/log_formatter.cc

CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_formatter.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/log/log_formatter.cc > CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.i

CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_formatter.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/log/log_formatter.cc -o CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.s

CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o: ../sylar/log/log_appender.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_appender.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o -c /mnt/d/dev/sylar/sylar/log/log_appender.cc

CMakeFiles/sylar.dir/sylar/log/log_appender.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/log/log_appender.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_appender.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/log/log_appender.cc > CMakeFiles/sylar.dir/sylar/log/log_appender.cc.i

CMakeFiles/sylar.dir/sylar/log/log_appender.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/log/log_appender.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log_appender.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/log/log_appender.cc -o CMakeFiles/sylar.dir/sylar/log/log_appender.cc.s

CMakeFiles/sylar.dir/sylar/log/log.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/log/log.cc.o: ../sylar/log/log.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building CXX object CMakeFiles/sylar.dir/sylar/log/log.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/log/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/log/log.cc.o -c /mnt/d/dev/sylar/sylar/log/log.cc

CMakeFiles/sylar.dir/sylar/log/log.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/log/log.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/log/log.cc > CMakeFiles/sylar.dir/sylar/log/log.cc.i

CMakeFiles/sylar.dir/sylar/log/log.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/log/log.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/log/log.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/log/log.cc -o CMakeFiles/sylar.dir/sylar/log/log.cc.s

CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o: ../sylar/socket/fd_manager.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building CXX object CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/socket/fd_manager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o -c /mnt/d/dev/sylar/sylar/socket/fd_manager.cc

CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/socket/fd_manager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/socket/fd_manager.cc > CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.i

CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/socket/fd_manager.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/socket/fd_manager.cc -o CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.s

CMakeFiles/sylar.dir/sylar/socket/hook.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/socket/hook.cc.o: ../sylar/socket/hook.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building CXX object CMakeFiles/sylar.dir/sylar/socket/hook.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/socket/hook.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/socket/hook.cc.o -c /mnt/d/dev/sylar/sylar/socket/hook.cc

CMakeFiles/sylar.dir/sylar/socket/hook.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/socket/hook.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/socket/hook.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/socket/hook.cc > CMakeFiles/sylar.dir/sylar/socket/hook.cc.i

CMakeFiles/sylar.dir/sylar/socket/hook.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/socket/hook.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/socket/hook.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/socket/hook.cc -o CMakeFiles/sylar.dir/sylar/socket/hook.cc.s

CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o: ../sylar/thread/mutex.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building CXX object CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/thread/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o -c /mnt/d/dev/sylar/sylar/thread/mutex.cc

CMakeFiles/sylar.dir/sylar/thread/mutex.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/thread/mutex.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/thread/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/thread/mutex.cc > CMakeFiles/sylar.dir/sylar/thread/mutex.cc.i

CMakeFiles/sylar.dir/sylar/thread/mutex.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/thread/mutex.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/thread/mutex.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/thread/mutex.cc -o CMakeFiles/sylar.dir/sylar/thread/mutex.cc.s

CMakeFiles/sylar.dir/sylar/thread/thread.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/thread/thread.cc.o: ../sylar/thread/thread.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building CXX object CMakeFiles/sylar.dir/sylar/thread/thread.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/thread/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/thread/thread.cc.o -c /mnt/d/dev/sylar/sylar/thread/thread.cc

CMakeFiles/sylar.dir/sylar/thread/thread.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/thread/thread.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/thread/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/thread/thread.cc > CMakeFiles/sylar.dir/sylar/thread/thread.cc.i

CMakeFiles/sylar.dir/sylar/thread/thread.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/thread/thread.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/thread/thread.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/thread/thread.cc -o CMakeFiles/sylar.dir/sylar/thread/thread.cc.s

CMakeFiles/sylar.dir/sylar/util/util.cc.o: CMakeFiles/sylar.dir/flags.make
CMakeFiles/sylar.dir/sylar/util/util.cc.o: ../sylar/util/util.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building CXX object CMakeFiles/sylar.dir/sylar/util/util.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) -D__FILE__=\"sylar/util/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/sylar.dir/sylar/util/util.cc.o -c /mnt/d/dev/sylar/sylar/util/util.cc

CMakeFiles/sylar.dir/sylar/util/util.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sylar.dir/sylar/util/util.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/util/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /mnt/d/dev/sylar/sylar/util/util.cc > CMakeFiles/sylar.dir/sylar/util/util.cc.i

CMakeFiles/sylar.dir/sylar/util/util.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sylar.dir/sylar/util/util.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"sylar/util/util.cc\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /mnt/d/dev/sylar/sylar/util/util.cc -o CMakeFiles/sylar.dir/sylar/util/util.cc.s

# Object files for target sylar
sylar_OBJECTS = \
"CMakeFiles/sylar.dir/sylar/config/config.cc.o" \
"CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o" \
"CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o" \
"CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o" \
"CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o" \
"CMakeFiles/sylar.dir/sylar/log/log_level.cc.o" \
"CMakeFiles/sylar.dir/sylar/log/log_event.cc.o" \
"CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o" \
"CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o" \
"CMakeFiles/sylar.dir/sylar/log/log.cc.o" \
"CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o" \
"CMakeFiles/sylar.dir/sylar/socket/hook.cc.o" \
"CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o" \
"CMakeFiles/sylar.dir/sylar/thread/thread.cc.o" \
"CMakeFiles/sylar.dir/sylar/util/util.cc.o"

# External object files for target sylar
sylar_EXTERNAL_OBJECTS =

../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/config/config.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/fiber/fiber.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/fiber/iomanager.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/fiber/scheduler.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/fiber/timer.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/log/log_level.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/log/log_event.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/log/log_formatter.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/log/log_appender.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/log/log.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/socket/fd_manager.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/socket/hook.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/thread/mutex.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/thread/thread.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/sylar/util/util.cc.o
../lib/libsylar.so: CMakeFiles/sylar.dir/build.make
../lib/libsylar.so: CMakeFiles/sylar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/dev/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking CXX shared library ../lib/libsylar.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/sylar.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/sylar.dir/build: ../lib/libsylar.so

.PHONY : CMakeFiles/sylar.dir/build

CMakeFiles/sylar.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/sylar.dir/cmake_clean.cmake
.PHONY : CMakeFiles/sylar.dir/clean

CMakeFiles/sylar.dir/depend:
	cd /mnt/d/dev/sylar/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/dev/sylar /mnt/d/dev/sylar /mnt/d/dev/sylar/build /mnt/d/dev/sylar/build /mnt/d/dev/sylar/build/CMakeFiles/sylar.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/sylar.dir/depend

