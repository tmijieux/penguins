cmake_minimum_required(VERSION 2.8)

project(penguins)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

# basically this allow the installation folder to be moved anywhere
# and the library to still be found correctly

include(scripts/FindFreeGLUT.cmake)
find_package(freeglut QUIET)
if (NOT FREEGLUT_FOUND)
  message("Would be better with freeglut.")
  find_package(GLUT QUIET)
  if (NOT GLUT_FOUND)
    message("Would be better with freeglut (or glut).")
  else (NOT GLUT_FOUND)
    add_definitions(-DUSE_GL_DISPLAY__)
    add_definitions(-DFREEGLUT_FOUND)
  endif (NOT GLUT_FOUND)
else (NOT FREEGLUT_FOUND)
  add_definitions(-DUSE_GL_DISPLAY__)
  add_definitions(-DGLUT_FOUND)
endif (NOT FREEGLUT_FOUND)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if (NOT DEFINED CMAKE_ENABLE_C99)
  set(CMAKE_ENABLE_C99 "-std=gnu99")
endif(NOT DEFINED CMAKE_ENABLE_C99)

set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C99} -Wall") 

enable_testing()

include_directories(include)

add_subdirectory(utils)
add_subdirectory(display)
add_subdirectory(server)
add_subdirectory(map)
add_subdirectory(clients)

# stackoverflow.com/questions/8934295/
#  add-source-in-a-subdirectory-to-a-cmake-project
# http://www.cmake.org/Wiki/CMake/Editors/Emacs

