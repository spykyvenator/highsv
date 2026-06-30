#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#ifndef CLI
#include "./gtk/highsvWin.h"
#endif

void
die(const char *fmt, ...)
{
	va_list ap;
	int saved_errno;

	saved_errno = errno;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);

	if (fmt[0] && fmt[strlen(fmt)-1] == ':')
		fprintf(stderr, " %s", strerror(saved_errno));
	fputc('\n', stderr);

#ifdef CLI
	exit(1);
#endif
}

void*
h_malloc(size_t s)
{
    void* res = malloc(s);
    if (!res) {
        die("memory allocation failed of %d bytes", s);
    }
    return res;
}

/*
void*
highsv_insert(highsv_vector v, size_t index)
{
  if (index >= v.len) {
    void *tmp = h_malloc(v.size*v.len*2);
    memcpy(tmp, v.data, v.size*v.len);
    free(v.data);
    v.data = tmp;
    rowIndex = tmpIndex;
    for (size_t i = v.len; i < len*2; i++) {// allocate to zero
      rowVal[i] = 0;
      rowIndex[i] = 0;
    }
    len*=2;
  }
  v.data[index] = value;
}
*/
