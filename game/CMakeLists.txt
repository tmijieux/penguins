
set(_penguins_server_src
  map.c
  server.c
  tile.c
  move.c
  coord.c
  player.c
  client.c
  path.c
  main.c
  option.c

  display.c
  display_animator.c
  display_mouseclick.c
  display_history.c
  )

add_executable(
  penguins_server
  ${_penguins_server_src}
  )

target_include_directories(
  penguins_server
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )

target_compile_definitions(
  penguins_server
  PRIVATE
  DEBUG
  )

target_link_options(
  penguins_server
  PRIVATE
  # make symbol declared in penguins_server
  # accessible from loaded plugins :
  "-rdynamic"
  )

target_link_libraries(
  penguins_server
  #
  penguins_utils
  d3v
  m
  dl
  )

install(
  TARGETS penguins_server
  DESTINATION penguins
  )

install(
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/debug.sh
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
  DESTINATION penguins
  )

add_subdirectory(test)
