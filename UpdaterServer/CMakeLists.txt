cmake_minimum_required(VERSION 3.16)
project(UpdaterServer C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build/)

add_executable(UpdaterServer
        src/cJSON.h src/cJSON.c
        src/FileSystem.h src/FileSystem.c
        src/NetworkSystem.h src/NetworkSystem.c
        src/Main.c
)

target_link_libraries(UpdaterServer pthread)