#ifndef PENGUINS_SERVER_CLIENT_H
#define PENGUINS_SERVER_CLIENT_H

#define MAX_CLIENT 50

#include "utils/module_loading.h"
#include "penguins/client_interface.h"

typedef struct client {
    struct client_methods methods;
    peng_libhandle_t handle;
} Client;

void client_module_init(void);
void client_module_exit(void);

int client_get_client_count(void);
Client *client_get(int id);
const char *client_get_name(int client);

#endif // PENGUINS_SERVER_CLIENT_H
