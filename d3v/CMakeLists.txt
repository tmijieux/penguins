
set(D3V_SRC
  d3v.c
  scene.c
  camera.c
  light.c
  object.c
  texture.c
  model.c
  mouse_projection.c
)

add_library(d3v SHARED ${D3V_SRC})
target_link_libraries(d3v m GL GLU jpeg ${FREEGLUT_LIBRARIES} math)
install(TARGETS d3v DESTINATION penguins/lib)
