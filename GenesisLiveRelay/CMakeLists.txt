project("GenesisLiveRelay")

file(GLOB GENESISLIVERELAY_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.*
    ${CMAKE_CURRENT_SOURCE_DIR}/source/*.*
)

add_executable(${PROJECT_NAME} ${GENESISLIVERELAY_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Ash::Ash AshLogger::AshLogger AshObjects::AshObjects fmt::fmt GenesisLiveShared::GenesisLiveShared LibDataChannel::LibDataChannelStatic)
