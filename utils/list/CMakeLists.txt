add_library(list SHARED list.c)
target_link_libraries(list list_node)
install(TARGETS list DESTINATION penguins/lib)

add_subdirectory(test)