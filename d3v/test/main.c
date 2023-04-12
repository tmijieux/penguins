#include <stdio.h>
#include <stdlib.h>
#include <d3v.h>

int main(int argc, char *argv[])
{
    path_init(argv[0]);
    d3v_init(1);
    vec3 zero = {0};

    struct model *m = model_load_wavefront("penguin.obj");
    struct object *o = d3v_object_create(m, NULL, (vec3){0}, (vec3){0},
					 (vec3){0.3,0.3,0.3});
    d3v_add_object (o);

    d3v_start(&zero);
    printf("main_loop_returned\n");
    d3v_exit();

    printf("exit_returned\n");
    return EXIT_SUCCESS;
}

