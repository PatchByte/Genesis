set(ASH_ENABLE_LOGGER ON)
set(ASH_ENABLE_USAGE_OF_EXTERNAL_VENDORS ON)

add_subdirectory("Ash")
# add_subdirectory("zydis")
add_subdirectory("mem")
add_subdirectory("fmt")
include("nlohmann-json.cmake")

set(GLFW_BUILD_TESTS OFF)

if(GENESIS_BUILD_GUI)
    add_subdirectory("glfw")
    # Just adding this to easily see if any packages are missing
    # If you this is missing please install: https://vulkan.lunarg.com/sdk/home
    find_package(Vulkan REQUIRED)

    include("imgui.cmake")
    include("imgui-node-editor.cmake")
    include("imgui-file-dialog.cmake")
endif(GENESIS_BUILD_GUI)
