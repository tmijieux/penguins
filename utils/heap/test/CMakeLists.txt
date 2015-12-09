#TEST HEAP
add_executable(test_heap test_heap.c)
set_target_properties(test_heap PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_heap heap)
add_test(test_heap test_heap)
