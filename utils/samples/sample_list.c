#include <stdio.h>
#include <stdlib.h>
#include "list.h"

int main(int argc, char *argv[])
{
	struct list *list = list_create(F_DEFAULT);

	list_add_element(list, (void*) 4);
	list_add_element(list, (void*) 7);
	list_insert_element(list, 2, (void*) 3);

	for (int i = 1; i <= list_size(list); i++) {
		printf("%ld ", (long) list_get_element(list, i));
	}
	puts("");
	list_destroy(list);
	return EXIT_SUCCESS;
}
