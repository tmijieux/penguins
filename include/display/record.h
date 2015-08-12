#ifndef RECORD_H
#define RECORD_H

struct record;

struct record *record_create(int capacity);
void record_free(struct record *l);

int record_add(struct record *l, int tileSrc, int tileDest);

// Return 1 : cursor moved. 0 otherwise.
int record_next(struct record *l);
int record_previous(struct record *l);

int record_is_end(struct record *l);
int record_is_begin(struct record *l);

// Return -1 if there isn't data.
int record_get_current_src(struct record *l);
int record_get_current_dest(struct record *l);
int record_get_rewind_src(struct record *l);
int record_get_rewind_dest(struct record *l);

#endif
