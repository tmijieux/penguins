include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  "${CMAKE_CURRENT_SOURCE_DIR}/../server/include"
  )


add_library(penguins_client1 MODULE client1.c)
target_link_libraries(penguins_client1 penguins_utils)

# add_library(penguins_client2 MODULE client2.c client2_hack.c client2_hack.h)
# target_link_libraries(penguins_client2 penguins_utils)

add_library(penguins_client3 MODULE client3.c)
target_link_libraries(penguins_client3 penguins_utils)

add_library(penguins_client4 MODULE client4.c)
target_link_libraries(penguins_client4 penguins_utils penguins_display)

install(TARGETS penguins_client1 DESTINATION penguins/clients)
# install(TARGETS penguins_client2 DESTINATION penguins/clients)
install(TARGETS penguins_client3 DESTINATION penguins/clients)
install(TARGETS penguins_client4 DESTINATION penguins/clients)

