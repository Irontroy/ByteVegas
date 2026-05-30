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

# Audio wird gebraucht, damit die neuen Spiel-Sounds laufen
set(SFML_BUILD_AUDIO ON CACHE INTERNAL "")
set(SFML_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(SFML_BUILD_DOC OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(SFML)

# Unsere ausführbare Datei
add_executable(ByteVegas main.cpp Game.cpp Game.h)

# Wir linken Grafik-, Fenster-, System- und Audio-Module
target_link_libraries(ByteVegas PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

# DLLs kopieren
if(WIN32)
    set(BYTEVEGAS_RUNTIME_DLLS
        ${sfml_BINARY_DIR}/lib/sfml-graphics-d-2.dll
        ${sfml_BINARY_DIR}/lib/sfml-window-d-2.dll
        ${sfml_BINARY_DIR}/lib/sfml-system-d-2.dll
        ${sfml_BINARY_DIR}/lib/sfml-audio-d-2.dll
    )

    if(EXISTS "${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll")
        list(APPEND BYTEVEGAS_RUNTIME_DLLS "${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll")
    elseif(EXISTS "${sfml_BINARY_DIR}/extlibs/bin/x64/openal32.dll")
        list(APPEND BYTEVEGAS_RUNTIME_DLLS "${sfml_BINARY_DIR}/extlibs/bin/x64/openal32.dll")
    endif()

    add_custom_command(TARGET ByteVegas POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${BYTEVEGAS_RUNTIME_DLLS} $<TARGET_FILE_DIR:ByteVegas>
    )
endif()