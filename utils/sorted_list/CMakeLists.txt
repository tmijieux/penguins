add_library(slist SHARED sorted_list.c)
target_link_libraries(slist list_node)
install(TARGETS slist DESTINATION penguins/lib)

add_subdirectory(test)