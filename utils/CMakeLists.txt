
file(GLOB _utils_hdrs include/utils/*.h)
file(GLOB _utils_src *.c)

add_library(penguins_utils
  # SHARED
  ${_utils_src}
  ${_utils_hdrs}
)
if (UNIX)
  target_link_libraries(penguins_utils m)
endif (UNIX)


target_include_directories(
  penguins_utils
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include"
  INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

install(TARGETS penguins_utils DESTINATION penguins/lib)

