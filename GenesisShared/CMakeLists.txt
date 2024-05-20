project("GenesisShared")

file(GLOB GENESISSHARED_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*/*.*
)

add_library(${PROJECT_NAME} STATIC ${GENESISSHARED_SOURCE_FILES})
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PUBLIC Ash::Ash AshLogger::AshLogger AshObjects::AshObjects fmt::fmt mem)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)