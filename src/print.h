#ifndef PRINT_H
#define PRINT_H

void
printSol(const void *mod)
{
  char text[kHighsMaximumStringLength];
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow];// I tought I had to malloc this??
  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);

  puts("values:");
  for (uint8_t i = 0; i < numCol; i++){
    Highs_getColName(mod, i, text);
    printf("i: %d, %s col: %lf\n",
      i, text, col_value[i]);
  }
}

#ifdef DEBUG
void
printModel(const void* mod)
{
  const HighsInt numCol = Highs_getPresolvedNumCol(mod), numRow = Highs_getPresolvedNumRow(mod);
  double offset;

  const double inf = Highs_getInfinity(mod);

  Highs_getObjectiveOffset(mod, &offset);
  printf("offset: %lf\n numCol: %d, numRow: %d\n", 
    offset,
    numCol, numRow
    );
}
#endif


#endif
