
# stackoverflow.com/questions/8934295
#      /add-source-in-a-subdirectory-to-a-cmake-project
# http://www.cmake.org/Wiki/CMake/Editors/Emacs

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../include")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(IPATH "${CMAKE_INSTALL_PREFIX}/penguins/")
set(QIPATH "\"${IPATH}\"")

set(CMAKE_INSTALL_RPATH
  "${IPATH}lib/"
  "${IPATH}clients/"
  "${IPATH}map/"
  )

add_custom_target(check
 # COMMAND env LD_LIBRARY_PATH=${PROJECT_SOURCE_DIR}/install/libs
 ${CMAKE_CTEST_COMMAND} --output-on-failure
 )

## TEST TILE
add_executable(test_tile test_tile.c ../tile.c)
target_include_directories(test_tile PRIVATE ../include)
set_target_properties(test_tile PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_tile penguins_utils)
add_test(test_tile test_tile)

## TEST COORD
add_executable(test_coord test_coord.c ../coord.c)
target_include_directories(test_coord PRIVATE ../include)
set_target_properties(test_coord PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_coord m penguins_utils)
add_test(test_coord test_coord)


## TEST MOVE
add_executable(test_move test_move.c ../move.c)
target_include_directories(test_move PRIVATE ../include)
set_target_properties(test_move PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_move penguins_utils dl m )
add_test(test_move test_move)

## TEST INIT_SERVER

set(_test_init_server_src
  ../server.c
  ../tile.c
  ../move.c
  ../player.c
  ../client.c
  ../map.c
  ../coord.c
  ../path.c
  )

add_executable(
  test_init_server
  test_init_server.c
  ${_test_init_server_src}
  )
target_include_directories(test_init_server PRIVATE ../include)
set_target_properties(
  test_init_server
  PROPERTIES
  COMPILE_FLAGS "-DTEST -DSERVER_INSTALL_PATH=${QIPATH}"
  )

target_link_libraries(
  test_init_server
  dl
  m
  penguins_display
  penguins_utils
  )

add_test(test_init_server test_init_server)

