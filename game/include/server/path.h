#ifndef PATH_H
#define PATH_H

extern int binary_dir;

// return filedescriptor to directory of assets
int path_init(const char *str);
void path_exit(void);

#endif //PATH_H
