add_subdirectory(penrose)
###
macro(BUILD_MAP name)
  add_library(map_${name} MODULE map_${name}.c)
  install(TARGETS map_${name} DESTINATION penguins/map)
endmacro(BUILD_MAP)
###
macro(LINK_MAP name)
  target_link_libraries(map_${name} ${ARGN})
endmacro(LINK_MAP)
### maps:
BUILD_MAP(square)
BUILD_MAP(hexagon)
BUILD_MAP(triangle)
BUILD_MAP(octogon)
BUILD_MAP(penrose)
LINK_MAP(penrose penrose)
