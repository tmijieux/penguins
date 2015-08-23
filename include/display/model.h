#ifndef MODEL_IFACE_H
#define MODEL_IFACE_H

struct model;

struct model *model_load_wavefront(const char *path);

#endif //MODEL_IFACE_H
