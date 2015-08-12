add_library(penrose SHARED penrose.c)
target_link_libraries(penrose fibonacci m)
install(TARGETS penrose DESTINATION penguins/lib)
