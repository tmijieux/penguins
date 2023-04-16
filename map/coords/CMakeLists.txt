
add_library(penguins_mapcoord
  coord.c
  coord.h
  )
target_link_libraries(penguins_mapcoord penguins_utils)
target_include_directories(penguins_mapcoord
  INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  )


## TEST COORD
add_executable(test_coord test_coord.c)
target_compile_definitions(test_coord PRIVATE TEST)
target_link_libraries(test_coord penguins_mapcoord)
add_test(test_coord test_coord)

