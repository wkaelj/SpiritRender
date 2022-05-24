#Spirit Render Engine
A vulkan rendering engine. It should be cross platform, and there are utilties to convert file paths to the local system style. Currently there is no platform header for windows, which may cause problems on windows systems.

Not finished yet, cannot actually do anything.

#Cloning
You must clone the repository using `git clone [link] --recurse-submodules`, then copy the libs/SPIRV-Headers into the right folder of libs/SPIRV-Tools, otherwise it will get angry and won't compile.

##Build Instructions
Must have the VulkanSDK and glfw installed. Probably only works on MacOS and Linux, and I have only tested
it on Arch(Manjaro) Linux.
 1. run `make cmake` to run cmake and compile the shaders
 2. run `make` to compile the executable
 2. run `make run` to run program, or `make debug` to run the program with gdb.

##Syntax
This is the syntax I am using
 funcName () - functions
 func_name() - function macro
 TypeName - typedef struct
 t_TypeName - normal struct
 EnumName - typedef enum
 e_EnumName - normal enum
 CONST_NAME - constant or definition
