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
CMAKE_SOURCE_DIR = /home/kael/Code/SpiritRender

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kael/Code/SpiritRender

# Include any dependencies generated for this target.
include libs/CMakeFiles/libs.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include libs/CMakeFiles/libs.dir/compiler_depend.make

# Include the progress variables for this target.
include libs/CMakeFiles/libs.dir/progress.make

# Include the compile flags for this target's objects.
include libs/CMakeFiles/libs.dir/flags.make

# Object files for target libs
libs_OBJECTS =

# External object files for target libs
libs_EXTERNAL_OBJECTS = \
"/home/kael/Code/SpiritRender/libs/shaderc/libshaderc/CMakeFiles/shaderc.dir/src/shaderc.cc.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/context.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/init.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/input.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/monitor.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/platform.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/vulkan.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/window.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/egl_context.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/osmesa_context.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/null_init.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/null_monitor.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/null_window.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/null_joystick.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/posix_module.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/posix_time.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/posix_thread.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/x11_init.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/x11_monitor.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/x11_window.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/xkb_unicode.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/glx_context.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/linux_joystick.c.o" \
"/home/kael/Code/SpiritRender/libs/glfw/src/CMakeFiles/glfw.dir/posix_poll.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/euler.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/affine.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/io.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/quat.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/cam.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/vec2.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/vec3.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/vec4.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/ivec2.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/ivec3.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/ivec4.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/mat2.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/mat3.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/mat4.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/plane.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/frustum.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/box.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/project.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/sphere.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/ease.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/curve.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/bezier.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/ray.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/affine2d.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_lh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_rh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_lh_no.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_rh_no.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_lh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_rh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_lh_no.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_rh_no.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_lh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_rh_zo.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_lh_no.c.o" \
"/home/kael/Code/SpiritRender/libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_rh_no.c.o"

libs/liblibs.so: libs/shaderc/libshaderc/CMakeFiles/shaderc.dir/src/shaderc.cc.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/context.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/init.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/input.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/monitor.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/platform.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/vulkan.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/window.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/egl_context.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/osmesa_context.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/null_init.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/null_monitor.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/null_window.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/null_joystick.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/posix_module.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/posix_time.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/posix_thread.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/x11_init.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/x11_monitor.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/x11_window.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/xkb_unicode.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/glx_context.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/linux_joystick.c.o
libs/liblibs.so: libs/glfw/src/CMakeFiles/glfw.dir/posix_poll.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/euler.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/affine.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/io.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/quat.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/cam.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/vec2.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/vec3.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/vec4.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/ivec2.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/ivec3.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/ivec4.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/mat2.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/mat3.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/mat4.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/plane.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/frustum.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/box.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/project.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/sphere.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/ease.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/curve.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/bezier.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/ray.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/affine2d.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_lh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_rh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_lh_no.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/persp_rh_no.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_lh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_rh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_lh_no.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/ortho_rh_no.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_lh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_rh_zo.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_lh_no.c.o
libs/liblibs.so: libs/cglm/CMakeFiles/cglm.dir/src/clipspace/view_rh_no.c.o
libs/liblibs.so: libs/CMakeFiles/libs.dir/build.make
libs/liblibs.so: libs/shaderc/libshaderc/libshaderc.a
libs/liblibs.so: libs/glfw/src/libglfw3.a
libs/liblibs.so: libs/cglm/libcglm.so.0.8.6
libs/liblibs.so: libs/shaderc/libshaderc_util/libshaderc_util.a
libs/liblibs.so: libs/glslang/glslang/libglslang.a
libs/liblibs.so: libs/glslang/hlsl/libHLSL.a
libs/liblibs.so: libs/spirv-tools/source/opt/libSPIRV-Tools-opt.a
libs/liblibs.so: libs/glslang/SPIRV/libSPIRV.a
libs/liblibs.so: libs/glslang/glslang/libMachineIndependent.a
libs/liblibs.so: libs/glslang/glslang/OSDependent/Unix/libOSDependent.a
libs/liblibs.so: libs/glslang/OGLCompilersDLL/libOGLCompiler.a
libs/liblibs.so: libs/glslang/glslang/libGenericCodeGen.a
libs/liblibs.so: libs/spirv-tools/source/libSPIRV-Tools.a
libs/liblibs.so: /usr/lib64/librt.a
libs/liblibs.so: /usr/lib64/libm.so
libs/liblibs.so: libs/CMakeFiles/libs.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kael/Code/SpiritRender/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking CXX shared library liblibs.so"
	cd /home/kael/Code/SpiritRender/libs && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libs.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libs/CMakeFiles/libs.dir/build: libs/liblibs.so
.PHONY : libs/CMakeFiles/libs.dir/build

libs/CMakeFiles/libs.dir/clean:
	cd /home/kael/Code/SpiritRender/libs && $(CMAKE_COMMAND) -P CMakeFiles/libs.dir/cmake_clean.cmake
.PHONY : libs/CMakeFiles/libs.dir/clean

libs/CMakeFiles/libs.dir/depend:
	cd /home/kael/Code/SpiritRender && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kael/Code/SpiritRender /home/kael/Code/SpiritRender/libs /home/kael/Code/SpiritRender /home/kael/Code/SpiritRender/libs /home/kael/Code/SpiritRender/libs/CMakeFiles/libs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libs/CMakeFiles/libs.dir/depend

