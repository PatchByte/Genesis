project("GenesisMerge")

find_package(Vulkan REQUIRED)

file(GLOB GENESISMERGE_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp
)

add_executable(${PROJECT_NAME} ${GENESISMERGE_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE Ash::Ash AshLogger::AshLogger glfw Vulkan::Vulkan ImGui::ImGui GenesisShared::GenesisShared GenesisRenderer::GenesisRenderer)
