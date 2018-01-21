cmake_minimum_required (VERSION 2.6)
project (FPGE)

include_directories ("${PROJECT_SOURCE_DIR}/loadpng-1.5")
include_directories ("${PROJECT_SOURCE_DIR}/src")

file(GLOB SRC src/*.c)
file(GLOB PNG loadpng-1.5/*.c)

add_executable(fpge ${SRC} ${PNG})

target_link_libraries (fpge png z alleg X11 Xext Xxf86vm Xpm Xcursor pthread)
