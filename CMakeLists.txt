cmake_minimum_required(VERSION 3.26)
project("Genesis-Root")

option(GENESIS_BUILD_GUI "" ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory("Vendors")
add_subdirectory("GenesisShared")
add_subdirectory("GenesisOutput")
add_subdirectory("GenesisLiveShared")
add_subdirectory("GenesisLiveRelay")

if(GENESIS_BUILD_GUI)
    add_subdirectory("GenesisRenderer")
    add_subdirectory("GenesisEditor")
endif(GENESIS_BUILD_GUI)
