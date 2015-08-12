set(CMAKE_INSTALL_RPATH 
  "\$ORIGIN/lib/:"
  "\$ORIGIN/clients/:"
  "\$ORIGIN/map/"
  )

set(PENG_SERVER_SRC
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
  
add_executable(penguins_server ${PENG_SERVER_SRC})
target_link_libraries(penguins_server m dl graph log display list)
install(TARGETS penguins_server DESTINATION penguins)

add_subdirectory(test)
