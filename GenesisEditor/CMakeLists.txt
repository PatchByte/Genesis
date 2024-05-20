project("GenesisEditor")

find_package(Vulkan REQUIRED)

file(GLOB GENESISEDITOR_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.*
)

add_executable(${PROJECT_NAME} ${GENESISEDITOR_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC Ash::Ash AshLogger::AshLogger glfw Vulkan::Vulkan ImGui::ImGui GenesisShared::GenesisShared GenesisRenderer::GenesisRenderer GenesisOutput::GenesisOutput ImGuiNodeEditor::ImGuiNodeEditor ImGuiFileDialog::ImGuiFileDialog)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Because almost everything that microsoft produces is garbage that is being accepted by society because everyone uses it
if(MSVC OR MINGW)
    target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${Vulkan_LIBRARIES})
endif(MSVC OR MINGW)

if(MINGW)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static)
endif(MINGW)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/"
        "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>/resources"
)