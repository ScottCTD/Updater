cmake_minimum_required(VERSION 3.19)
project(UpdaterClient C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build/)

add_executable(UpdaterClient
        src/dirent.h
        src/cJSON.h src/cJSON.c
        src/FileSystem.h src/FileSystem.c
        src/NetworkSystem.h src/NetworkSystem.c
        src/Main.c
)

target_link_libraries(UpdaterClient ws2_32)
