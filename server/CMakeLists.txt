
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
  penguins_display
  penguins_utils
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
