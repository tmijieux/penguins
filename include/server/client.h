#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#define MAX_CLIENT 50

void client_init(void);
void client_exit(void);

int client_get_client_count(void);
void *client_get_method(int client_id, const char *method_name);
const char *client_get_name(int client);

#endif //SERVER_CLIENT_H
