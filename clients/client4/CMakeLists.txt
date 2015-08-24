add_library(client4 MODULE client4.c)
target_link_libraries(client4 heap list)
install(TARGETS client4 DESTINATION penguins/clients)
