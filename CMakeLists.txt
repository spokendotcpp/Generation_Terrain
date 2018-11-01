cmake_minimum_required(VERSION 3.5)

project(GenerationTerrain)

set(exe_name "fields_generation")

# CMAKE QT OPTIONS
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# CMAKE CPP FLAGS
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors")

# OPENGL PREFERENCE LIBS
set(OpenGL_GL_PREFERENCE "LEGACY")

# PACKAGES REQUIRED
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
    src/meshobject.cpp
    src/drawableobject.cpp
    src/axis.cpp
)

# HEADERS FILES
set(HEADERS
    include/mainwindow.h
    include/meshviewerwidget.h
    include/meshobject.h
    include/drawableobject.h
    include/axis.h
)

# EXE + LINKING
add_executable(
    ${exe_name}
    ${SOURCES}
    ${HEADERS}
    collections.qrc
)

target_link_libraries(
    ${exe_name}
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL
    OpenMeshCore
    OpenMeshTools
    ${OPENGL_LIBRARIES}
)
