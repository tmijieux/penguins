## TEST LIST
add_executable(test_list test_list.c)
set_target_properties(test_list PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_list list)
add_test(test_list test_list)
