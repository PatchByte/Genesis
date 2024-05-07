project("ImGuiNodeEditor")

file(GLOB IMGUINODEEDITOR_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor/*.cpp
)

add_library(${PROJECT_NAME} STATIC ${IMGUINODEEDITOR_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-node-editor)
target_link_libraries(${PROJECT_NAME} PUBLIC ImGui::ImGui)