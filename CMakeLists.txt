cmake_minimum_required(VERSION 3.20)

project(penguins)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR}/install)

# basically this allow the installation folder to be moved anywhere
# and the library to still be found correctly

add_definitions(-DUSE_GL_DISPLAY__)

# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-allow-shlib-undefined -Wl,-z,defs")

if (NOT DEFINED CMAKE_ENABLE_C11)
  set(CMAKE_ENABLE_C11 "-std=gnu11")
endif(NOT DEFINED CMAKE_ENABLE_C11)

#set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C11} -Wall -Wl,-Xld -Wl,--disable-new-dtags")
#set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C11} -Wall -Wl,--disable-new-dtags")
set(CMAKE_C_FLAGS "${CMAKE_ENABLE_C11} -Wall ")

enable_testing()

add_subdirectory(utils)

add_subdirectory(map)
add_subdirectory(clients)
add_subdirectory(d3v)
add_subdirectory(game)

add_subdirectory(assets)

# stackoverflow.com/questions/8934295/
#  add-source-in-a-subdirectory-to-a-cmake-project
# http://www.cmake.org/Wiki/CMake/Editors/Emacs

