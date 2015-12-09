
set(DISPLAY_SRC
  animator.c
  display.c
  record.c
  dpenguin.c
  dtile.c
  mouse_click.c
)
find_package(Threads)

add_library(display SHARED ${DISPLAY_SRC})
target_link_libraries(display d3v m GL GLU  jpeg
  ${CMAKE_THREAD_LIBS_INIT} ${FREEGLUT_LIBRARIES} math)

install(TARGETS display DESTINATION penguins/lib)

