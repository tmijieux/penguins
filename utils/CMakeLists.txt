

set(_utils_src
  fibonacci.c
  graph.c
  heap.c
  list.c
  list_node.c
  log.c
  math.c
  sorted_list.c
  util.c
  )

add_library(penguins_utils
  SHARED
  ${_utils_src}
  )

target_link_libraries(
  penguins_utils
  m
  )

target_include_directories(
  penguins_utils
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )


install(TARGETS penguins_utils DESTINATION penguins/lib)

