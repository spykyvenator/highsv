#include "pt.h"
#include "../../util.h"

extern size_t numCol;

static sm*
appendSm(sm* a, double val, int index)
{
  for (size_t i = 0; i < a->rI; i++) {
      if (a->indices[i] == index) {
          a->vals[i] += val;
          return a;
      }
  }
  if (index >= a->rL) {
    double *tmpVal = (double*) h_malloc(sizeof(double)*a->rL*2);
    int *tmpIndex = (int*) h_malloc(sizeof(int)*a->rL*2);
    memcpy(tmpVal, a->vals, sizeof(double)*a->rL);
    memcpy(tmpIndex, a->indices, sizeof(int)*a->rL);
    free(a->vals);
    free(a->indices);
    a->vals = tmpVal;
    a->indices = tmpIndex;
    for (size_t i = a->rL; i < a->rL*2; i++) {// allocate to zero
      a->vals[i] = 0;
      a->indices[i] = -1;
    }
    a->rL*=2;
  }
  a->vals[a->rI] += val;
  a->indices[a->rI] = index;
  a->numNz++; // right now should still check if index is already present
  a->rI++;
#ifdef DEBUG
  printf("appended: %lf at index: %d\n", val, index);
#endif
  return a;
}

sm*
mergeSm(sm* a, sm* b)
{
    a->offset = b->offset - a->offset;
    if (b->numNz == 0)
        return a;
    char found = 0;
    for (size_t i = 0; i < a->numNz; i++) {
        for (size_t j = 0; j < b->numNz; j++) {
            if (a->indices[i] == b->indices[j]) {
                a->vals[i] -= b->vals[j];
                found = 1;
                break;
            }
        }
        if (!found) {
            appendSm(a, b->vals[b->numNz], b->indices[b->numNz]);
        }
        found = 0;
    }
    return a;
}

void
apply_sm(sm *a, void *model)
{
}

void
destroy_sm(sm *a)
{
    free(a->vals);
    free(a->indices);
    free(a);
}

void
print_sm(sm *a)
{
    printf("printing sm: size: %ld, rL:  %ld\n", a->numNz, a->rL);
    for (size_t i = 0; i < a->numNz; i++) {
            printf("index: %d is %f\n", a->indices[i], a->vals[i]);
    }
}

sm*
init_sm(void)
{
	sm* res = (sm*) h_malloc(sizeof(sm));
	res->offset = 0.0;
        res->numNz = 0;
        res->rI = 0;
	res->rL = 2;// row length
	res->vals = h_malloc(sizeof(double)*res->rL);
	res->indices = h_malloc(sizeof(size_t)*res->rL);
        for (size_t i = 0; i < res->rL; i++) {// zero init
                res->vals[i] = 0;
                res->indices[i] = -1;
        }
	return res;
}



/*
  return the column index of a variable name.
  add it if it is not present
*/
size_t
findIndex(void *mod, const char *text)
{
  int64_t index;
  if (highsv_getColByName(mod, text, &index) == HIGHSV_STATUS_ERROR) {// returns Error when col does not exist
    #ifdef DEBUG
    printf("adding %s to index\n", text);
    #endif
    index = highsv_getNumCol(mod);
    highsv_addVar(mod);
    highsv_passColName(mod, index, text);
    numCol++;
  }
  return (size_t) index;
}

void
setCost(void *mod, const char *var, const double val)
{
        int64_t nz = highsv_getNumNz(mod);
	const size_t index = findIndex(mod, var);
        int64_t nnz, ms, mi[nz], nc;

        double cost, l, u, mv[nz];
        highsv_getColsByRange(mod, (int64_t) index,// better use a statement for this
                              (int64_t) index, &nc,
                              &cost, &l, &u,
                              &nnz, &ms,
                              mi, mv);
	highsv_changeColCost(mod, index, cost+val);
}

sm*
setVal(void *mod, sm *a, const char *var, const double val)
{
	const size_t index = findIndex(mod, var);
        appendSm(a, val, index);
        return a;
}
