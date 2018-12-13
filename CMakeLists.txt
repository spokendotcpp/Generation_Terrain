cmake_minimum_required(VERSION 3.5)

project(Generation_Terrain)
set(exe_name "fields")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# CMAKE QT OPTIONS
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# CMAKE CPP FLAGS
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic-errors")

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
    src/drawableobject.cpp
    src/axis.cpp
    src/light.cpp
    src/arcball.cpp
)

# HEADERS FILES
set(HEADERS
    include/mainwindow.h
    include/meshviewerwidget.h
    include/drawableobject.h
    include/axis.h
    include/light.h
    include/arcball.h
)

set(UI_FORMS
    include/mainwindow.ui
)

# EXE + LINKING
add_executable(
    ${exe_name}
    ${SOURCES}
    ${HEADERS}
    ${UI_FORMS}
)

# -l
target_link_libraries(
    ${exe_name} PUBLIC

    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::OpenGL

    ${OPENGL_LIBRARIES}
)
