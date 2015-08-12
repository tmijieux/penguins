#include <stdio.h>
#include <stdlib.h>
#include <sorted_list.h>

static int compare_int(void *a_, void *b_)
{
	return (int)((unsigned long) a_ - (unsigned long) b_);
}

void print_slist(struct sorted_list *sl)
{
	int s = slist_size(sl);
	for (int i = 1; i <= s; i ++) {
		long a = (long) slist_get_data(sl, i);
		printf("%ld ", a);
	}
	puts("");
}

int main(int argc, char *argv[])
{
	struct sorted_list *sl = slist_create(0, &compare_int);

	slist_add_value(sl, (void*) 13L);
	print_slist(sl);
	
	slist_add_value(sl, (void*) 2L);
	print_slist(sl);
		
	slist_add_value(sl, (void*) 7L);
	print_slist(sl);
	
	slist_add_value(sl, (void*) 15L);
	print_slist(sl);
	
	slist_add_value(sl, (void*) 1L);
	print_slist(sl);

		
	slist_add_value(sl, (void*) 15L);
	print_slist(sl);
	
	slist_destroy(sl);
	return EXIT_SUCCESS;
}

