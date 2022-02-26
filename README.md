#Spirit Render Engine
A rewrite of SimpleVulkanEngine.

##Build Instructions
Must have the VulkanSDK and glfw installed. Probably only works on MacOS and Linux, and I have only tested
it on Arch(Manjaro) Linux.
 1. run `./genmake.sh` to run cmake and compile the shaders
 2. run `./runmake.sh` to compile the executable
 2. run `./test.sh` to run program, or `./debug.sh` to run the program with gdb.

##Syntax
This is the syntax I am using
 funcName () - functions
 func_name() - function macro
 TypeName - typedef struct
 t_TypeName - normal struct
 EnumName - typedef enum
 e_EnumName - normal enum
 CONST_NAME - constant or definition