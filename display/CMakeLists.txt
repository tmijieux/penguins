
set(_display_src
  animator.c
  display.c
  record.c
  dpenguin.c
  dtile.c
  mouse_click.c
)
find_package(Threads)

add_library(penguins_display
  SHARED
  ${_display_src}
  )

target_include_directories(
  penguins_display
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

target_link_libraries(penguins_display
  d3v
  m
  GL
  GLU
  glut
  jpeg
  ${CMAKE_THREAD_LIBS_INIT}
  ${FREEGLUT_LIBRARIES}
  )

install(TARGETS penguins_display DESTINATION penguins/lib)

