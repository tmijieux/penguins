add_library(heap SHARED heap.c)
install(TARGETS heap DESTINATION penguins/lib)

add_subdirectory(example)
add_subdirectory(test)
