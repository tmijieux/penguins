
add_executable(heap_example main.c)
target_link_libraries(heap_example heap list)
install(TARGETS heap_example DESTINATION examples)