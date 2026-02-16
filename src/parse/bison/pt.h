#ifndef SM_H
#define SM_H

typedef struct {
        double *vals;
        int *indices;
        double offset;
        size_t rL;
} sm;

sm* init_sm(void);
#endif
