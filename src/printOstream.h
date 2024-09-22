#ifndef PRINTO_H
#define PRINTO_H
extern int numRow, numCol;

void
printEmpty(const void *mod, GOutputStream* ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Your model appears empty\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printInfeasible(const void *mod, GOutputStream* ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Your model is infeasible\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printValues(const void *mod, GOutputStream *ostream, gsize bw, GError *error)
{
  const double objectiveVal = Highs_getObjectiveValue(mod);
  char text[kHighsMaximumStringLength];
  const char RowText[] = "\n\nRow\t\t Slack or Surplus\tDual Price\n";
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow],
        col_reduced[numRow];
  HighsInt col_index[numRow];

  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);

  if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
          "Objective value:\t\t%lf\nTotal Variables:\t\t\t%d\nTotal Constraints:\t\t%d\nTotal nonzeros:\t\t\t%d\n\nVariable\tValue\t\tReduced Cost\n",
          objectiveVal,
          numCol, numRow, Highs_getNumNz(mod))) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }

  for (uint8_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;
    HighsInt num_nz, reducedIndex;
    if (Highs_getReducedColumn(mod, i, col_reduced, &num_nz, col_index) == kHighsStatusError)
      break;

    printf("num_nz: %d\n", num_nz);
    for (reducedIndex = 0; reducedIndex < num_nz; reducedIndex++){
      printf("col_reduced: %lf, col_index: %d\n", col_reduced[reducedIndex], col_index[reducedIndex]);
      if (col_index[reducedIndex] == i){
        break;
      }
    }
    printf("reducedIndex: %d\n", reducedIndex);

    if (col_value[i] == 0.0 && signbit(col_value[i]))// remove negative 0's
        col_value[i] += 0.0;

    if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
            "%s\t\t%lf\t\t\t%lf\n",
            text, col_value[i], col_reduced[reducedIndex])) {
        g_printerr("Error writing to file: %s\n", error->message);
        g_clear_error(&error);
    }
  }

  if (!g_output_stream_write_all(ostream, RowText, 
        sizeof(RowText) - 1, &bw, NULL, &error)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }

  for (uint8_t i = 0; i < numRow; i++){
    if (Highs_getRowName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;

    if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
            "%s\t\t%lf\t\t\t%lf\n",
            text, 0.0, 0.0)) {
        g_printerr("Error writing to file: %s\n", error->message);
        g_clear_error(&error);
    }
  }
}

void
printUnbounded(const void *mod, GOutputStream *ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Your model is unbounded\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }

  printValues(mod, ostream, bw, error);

}

void
printError(const void *mod, GOutputStream *ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Something went wrong\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printOptimal(const void *mod, GOutputStream *ostream){
  gsize bw;
  GError *error = NULL;
  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Global optimal solution found:\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
  printValues(mod, ostream, bw, error);
}

void
printSolToFile(const void *mod, GOutputStream* ostream) {
  HighsInt status = Highs_getModelStatus(mod);


  if (status == kHighsModelStatusNotset
    || status == kHighsModelStatusLoadError
    || status == kHighsModelStatusModelError
    || status == kHighsModelStatusPresolveError
    || status == kHighsModelStatusSolveError
    || status == kHighsModelStatusPostsolveError
    || status == kHighsModelStatusTimeLimit
    || status == kHighsModelStatusIterationLimit
    || status == kHighsModelStatusUnknown
    || status == kHighsModelStatusSolutionLimit
    || status == kHighsModelStatusInterrupt)
    printError(mod, ostream);
  else if (status == kHighsModelStatusModelEmpty)
      printEmpty(mod, ostream);
  else if (status == kHighsModelStatusOptimal)
      printOptimal(mod, ostream);
  else if (status == kHighsModelStatusInfeasible 
      || status == kHighsModelStatusUnboundedOrInfeasible)
    printInfeasible(mod, ostream);
  else if (status == kHighsModelStatusUnbounded)
    printUnbounded(mod, ostream);
}

#endif
