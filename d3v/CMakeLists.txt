
set(_d3v_src
  d3v.c
  scene.c
  camera.c
  light.c
  object.c
  texture.c
  model.c
  shader.c
  mouse_projection.c
  )

add_library(
  d3v
  SHARED
  ${_d3v_src}
  )

target_include_directories(
  d3v
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  )


target_link_libraries(d3v
  penguins_utils
  ####
  m
  jpeg
  ####
  GL # OpenGL core
  GLU # GL utility
  # X11 (X11 window management)
  glfw # GL FrameWork (portable cross-platform window management) (alternative to GLUT)
  )
install(TARGETS d3v DESTINATION penguins/lib)
