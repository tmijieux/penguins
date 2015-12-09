add_library(client2 MODULE client2.c hack.c)
target_link_libraries(client2 heap list)
install(TARGETS client2 DESTINATION penguins/clients)
