add_library(client11 MODULE client1.c)
target_link_libraries(client11 heap list)

add_library(client12 MODULE client1.c)
target_link_libraries(client12 heap list)

install(TARGETS client11 DESTINATION penguins/clients)
install(TARGETS client12 DESTINATION penguins/clients)
