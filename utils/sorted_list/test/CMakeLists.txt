## TEST SLIST
add_executable(test_slist test_slist.c)
set_target_properties(test_slist PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_slist slist)
add_test(test_slist test_slist)
