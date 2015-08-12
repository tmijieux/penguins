
set(DISPLAY_SRC
  camera.c
  animator.c
  display.c
  light.c
  record.c
  object.c
  texture.c
  model.c
  dpenguin.c
  dtile.c
)
find_package(Threads)

add_library(display SHARED ${DISPLAY_SRC})
target_link_libraries(display m GL GLU  jpeg
  ${CMAKE_THREAD_LIBS_INIT} ${FREEGLUT_LIBRARIES} math)

install(TARGETS display DESTINATION penguins/lib)

install(DIRECTORY models DESTINATION penguins)
install(DIRECTORY textures DESTINATION penguins)

