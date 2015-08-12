## TEST LIST
add_executable(test_list_node test_list_node.c)
set_target_properties(test_list_node PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_list_node list_node)
add_test(test_list_node test_list_node)
