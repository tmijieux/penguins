
include_directories(
  ../include
  ../game/include
  )

### maps:
add_library(map_square MODULE map_square.c)
add_library(map_hexagon MODULE map_hexagon.c)
add_library(map_triangle MODULE map_triangle.c)
add_library(map_octogon MODULE map_octogon.c)
add_library(map_penrose
  MODULE
  map_penrose.c
  penrose/penrose.c
  penrose/penrose.h
  )

target_link_libraries(map_square penguins_utils)
target_link_libraries(map_hexagon penguins_utils)
target_link_libraries(map_triangle penguins_utils)
target_link_libraries(map_octogon penguins_utils)
target_link_libraries(map_penrose penguins_utils)



install(TARGETS map_square DESTINATION penguins/map)
install(TARGETS map_hexagon DESTINATION penguins/map)
install(TARGETS map_triangle DESTINATION penguins/map)
install(TARGETS map_octogon DESTINATION penguins/map)
install(TARGETS map_penrose DESTINATION penguins/map)
