#ifndef PRINT_H
#define PRINT_H

void
printSol(const void *mod)
{
  const int64_t numCol = highsv_getPresolvedNumCol(mod), numRow = highsv_getPresolvedNumRow(mod);
  char text[kHighsMaximumStringLength];
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow];// I tought I had to malloc this??
  highsv_getSolution(mod, col_value, col_dual, row_value, row_dual);

  puts("values:");
  for (uint8_t i = 0; i < numCol; i++){
    highsv_getColName(mod, i, text);
    printf("i: %d, %s col: %lf\n",
      i, text, col_value[i]);
  }
}

#ifdef DEBUG
void
printModel(const void* mod)
{
  const int64_t numCol = highsv_getPresolvedNumCol(mod), numRow = highsv_getPresolvedNumRow(mod);
  double offset;

  offset = highsv_getObjectiveOffset(mod);
  puts("printing model: ");
  printf("offset: %lf\n numCol: %ld, numRow: %ld\n", 
    offset,
    numCol, numRow
    );
}
#endif
#endif
