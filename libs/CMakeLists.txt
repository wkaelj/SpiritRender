# consolidate all dependencies into a single shared/static library
# to either optimize performance or compile times

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_ENABLE_COPYRIGHT_CHECK OFF)

set(SPIRV_HEADERS_SKIP_INSTAL OFF)
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)

add_subdirectory(spirv-tools)
add_subdirectory(glslang)
add_subdirectory(shaderc)
add_subdirectory(liblist)
add_subdirectory(glfw)

add_library(deps SHARED
    $<TARGET_OBJECTS:shaderc>
    $<TARGET_OBJECTS:glfw>
    $<TARGET_OBJECTS:linkedlist>)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SHADERC_SKIP_INSTALL ON)
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_ENABLE_COPYRIGHT_CHECK OFF)

set(SPIRV_HEADERS_SKIP_INSTAL OFF)
set(SPIRV_HEADERS_SKIP_EXAMPLES ON)

target_link_libraries(deps PUBLIC
  shaderc
  linkedlist
  glfw
)