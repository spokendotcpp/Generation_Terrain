cmake_minimum_required(VERSION 3.5)

project(GenerationTerrain)

set(exe_name "fields_generation")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Set up std=c++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

find_package(
    Qt5 REQUIRED COMPONENTS
    Widgets
    Core
    Gui
    OpenGL
)

find_package(OpenGL REQUIRED)

# CPP SOURCES FILES
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/meshviewerwidget.cpp
)

# HEADERS FILES
set(HEADERS
    include/mainwindow.h
    include/meshviewerwidget.h
)

#EXE
add_executable(
    ${exe_name}
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(
    ${exe_name}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    ${OPENGL_gl_LIBRARY}
)
