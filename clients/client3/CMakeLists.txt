add_library(client31 MODULE client3.c)
target_link_libraries(client31 heap list)
#install(TARGETS client32 DESTINATION penguins/clients)

add_library(client32 MODULE client3.c)
target_link_libraries(client32 heap list)
#install(TARGETS client31 DESTINATION penguins/clients)

