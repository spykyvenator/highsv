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
  double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow];

  Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);

  if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
          "Objective value:\t\t%lf\nTotal Variables:\t\t\t%d\nTotal Constraints:\t\t%d\n\nVariable\tValue\t\t\tReduced Cost\n",
          objectiveVal,
          numCol, numRow)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }

  for (uint8_t i = 0; i < numCol; i++){
    if (Highs_getColName(mod, i, text) == kHighsStatusError)// stop printing if variable has no name
      break;

    if (col_value[i] == 0.0 && signbit(col_value[i]))// remove negative 0's
        col_value[i] += 0.0;

    if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
            "%s\t%lf\n",
            text, col_value[i])) {
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
