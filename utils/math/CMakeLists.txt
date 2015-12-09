add_library(math SHARED math.c)
target_link_libraries(math m)
install(TARGETS math DESTINATION penguins/lib)

add_library(fibonacci SHARED fibonacci.c)
install(TARGETS fibonacci DESTINATION penguins/lib)

add_subdirectory(test)
