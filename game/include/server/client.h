#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#define MAX_CLIENT 50

#include "penguins/client_interface.h"

struct client {
    struct client_methods methods;
    void *dl_handle;
};
typedef struct client Client;

void client_module_init(void);
void client_module_exit(void);

int client_get_client_count(void);
Client *client_get(int id);
const char *client_get_name(int client);

#endif // SERVER_CLIENT_H
