cmake_minimum_required(VERSION 3.5)

project(ImGuiFileDialog)

add_library(${PROJECT_NAME} STATIC
    ImGuiFileDialog/ImGuiFileDialog.cpp
    ImGuiFileDialog/ImGuiFileDialog.h
    ImGuiFileDialog/ImGuiFileDialogConfig.h
)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuiFileDialog)

if(UNIX)
    target_compile_options(${PROJECT_NAME} PUBLIC -Wno-unknown-pragmas)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC ImGui::ImGui)