add_library(list_node SHARED list_node.c)
install(TARGETS list_node DESTINATION penguins/lib)

add_subdirectory(test)