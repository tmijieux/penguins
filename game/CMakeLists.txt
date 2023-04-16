
file(GLOB _game_src *.c)
file(GLOB _game_hdrs ./include/**/*.h)

add_executable(
  penguins_server
  ${_game_src}
  ${_game_hdrs}
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
  # DEBUG
  )

target_link_options(
  penguins_server
  PRIVATE
  # make symbol declared in penguins_server
  # accessible from loaded plugins :
  "-rdynamic"
  )

target_link_libraries(penguins_server penguins_utils d3v)
if (UNIX)
  target_link_libraries(penguins_server m)
endif (UNIX)

install(
  TARGETS penguins_server
  DESTINATION penguins
  )

install(
  FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/launch.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/debug.sh
  ${CMAKE_CURRENT_SOURCE_DIR}/launch.bat
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
  DESTINATION penguins
  )

add_subdirectory(test)
