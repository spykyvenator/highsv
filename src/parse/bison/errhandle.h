#ifndef ERRHANDLE_H
#define ERRHANDLE_H
#include "../../util.h"
#ifdef CLI
#include <stdlib.h>
typedef struct errHandle errHandle;
struct errHandle {
    void (*pErr)(errHandle *self, const char *msg, int x, int y, int x2, int y2);
    void (*free)(errHandle *self);
};

static void
cli_errHandler(errHandle *self, const char *msg, int x, int y, int x2, int y2)
{
    die("ERROR: %d.%d - %d.%d: %s", x, y, x2, y2, msg, x, y, x2, y2);
}

static void
freeEh(errHandle *self)
{
    free(self);
}

static inline errHandle*
new_errHandle()
{
    errHandle *res = (errHandle*) h_malloc(sizeof(errHandle));
    res->pErr = cli_errHandler;
    res->free = freeEh;
    return res;
}
#elif GTK
#include <gtk/gtk.h>
#include "../../gtk/highsvWin.h"

typedef struct errHandle errHandle;
struct errHandle {
    void (*pErr)(errHandle *self, const char *msg, int x, int y, int x2, int y2);
    void (*free)(errHandle *self);
    GtkWidget *view;
    GtkTextBuffer *bfr;
};

static void
gtk_errHandler(errHandle *self, const char *msg, int x, int y, int x2, int y2)
{
    highsvShowError(msg, self->view, self->bfr, x, y, x2, y2);
}

static void
freeEh(errHandle *self)
{
    free(self);
}

static inline errHandle*
new_errHandle(GtkWidget *view, GtkTextBuffer *bfr)
{
    errHandle *res = (errHandle*) h_malloc(sizeof(errHandle));
    res->view = view;
    res->bfr = bfr;
    res->pErr = gtk_errHandler;
    res->free = freeEh;
    return res;
}
#else
typedef struct errHandle {
    void (*pErr)(errHandle *self, const char *msg);
} errHandle;
#endif
#endif
