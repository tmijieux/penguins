
file(GLOB _d3v_headers include/**/*.h)
file(GLOB _d3v_src *.c)

add_library(d3v STATIC
  ${_d3v_src}
  ${_d3v_headers}
  include/stb_image.h
  include/penguins_opengl.h
  )

target_include_directories(
  d3v
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/include"
  )


target_link_libraries(d3v
  penguins_utils
  ####
  ####
  glad
  # GL # OpenGL core
  ${GLU_LIBRARY} # GLU = GL utility
  # X11 (X11 window management)
  glfw::glfw # GL FrameWork (portable cross-platform window management) (alternative to GLUT)

  Freetype::Freetype
)
if (UNIX)
  target_link_libraries(d3v m)
endif (UNIX)

install(TARGETS d3v DESTINATION penguins/lib)
