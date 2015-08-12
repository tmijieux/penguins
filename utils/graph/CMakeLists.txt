add_library(graph SHARED graph.c)
target_link_libraries(graph slist)
install(TARGETS graph DESTINATION penguins/lib)

add_subdirectory(test)
