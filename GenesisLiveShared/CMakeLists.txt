project("GenesisLiveShared")

file(GLOB GENESISLIVESHARED_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.*

    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.*
)

add_library(${PROJECT_NAME} STATIC ${GENESISLIVESHARED_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Ash::Ash AshLogger::AshLogger AshObjects::AshObjects fmt::fmt LibDataChannel::LibDataChannelStatic nlohmann_json_patch::nlohmann_json_patch)