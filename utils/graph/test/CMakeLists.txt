#TEST GRAPH
add_executable(test_graph test_graph.c)
set_target_properties(test_graph PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_graph graph)
add_test(test_graph test_graph)
