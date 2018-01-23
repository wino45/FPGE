cmake_minimum_required (VERSION 2.6)
project (FPGE)

if ( WIN32 )
  include_directories ("${PROJECT_SOURCE_DIR}/loadpng-1.5")
endif ()
include_directories ("${PROJECT_SOURCE_DIR}/src")

file(GLOB SRC src/*.c)
if ( WIN32 )
  file(GLOB PNG loadpng-1.5/*.c)
endif ()

if ( WIN32 )
  add_executable(fpge ${SRC} ${PNG})
else ()
  add_executable(fpge ${SRC})
endif ()

if ( WIN32 )
  target_link_libraries (fpge png z alleg X11 Xext Xxf86vm Xpm Xcursor pthread)
else ()
  target_link_libraries (fpge loadpng z alleg X11 Xext Xxf86vm Xpm Xcursor pthread)
endif ()

