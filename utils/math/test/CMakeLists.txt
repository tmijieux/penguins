## TEST FIBONACCI
add_executable(test_fibonacci test_fibonacci.c ../fibonacci.c)
set_target_properties(test_fibonacci PROPERTIES COMPILE_FLAGS -DTEST)
add_test(test_fibonacci test_fibonacci)

## TEST MATH
add_executable(test_math test_math.c)
set_target_properties(test_math PROPERTIES COMPILE_FLAGS -DTEST)
target_link_libraries(test_math math)
add_test(test_math test_math)
