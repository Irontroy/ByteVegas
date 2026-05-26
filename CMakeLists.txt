cmake_minimum_required(VERSION 3.15)
project(ByteVegas VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        2.6.1
)

# Hier schalten wir Audio und andere unnötige Dinge ab, um dem Fehler zu entkommen
set(SFML_BUILD_AUDIO OFF CACHE INTERNAL "")
set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOC OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)

# Unsere ausführbare Datei
add_executable(ByteVegas main.cpp Game.cpp Game.h)

# Wir linken nur die funktionierenden Grafik-, Fenster- und System-Module
target_link_libraries(ByteVegas PRIVATE sfml-graphics sfml-window sfml-system)

# DLLs kopieren (ohne Audio)
if(WIN32)
    add_custom_command(TARGET ByteVegas POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window>
        $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:ByteVegas>
    )
endif()