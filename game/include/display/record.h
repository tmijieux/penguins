#ifndef DISPLAY_RECORD_H
#define DISPLAY_RECORD_H

typedef struct record record_t;

record_t* record_create(int capacity);
void record_free(record_t *l);

int record_add(record_t *l, int tileSrc, int tileDest);

// Return 1 : cursor moved. 0 otherwise.
int record_next(record_t *l);
int record_previous(record_t *l);

int record_is_end(record_t *l);
int record_is_begin(record_t *l);

// Return -1 if there isn't data.
int record_get_current_src(record_t *l);
int record_get_current_dest(record_t *l);
int record_get_rewind_src(record_t *l);
int record_get_rewind_dest(record_t *l);

#endif // DISPLAY_RECORD_H
