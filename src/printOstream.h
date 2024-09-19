//#ifdef PRINTO_H
//#define PRINTO_H
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
printUnbounded(const void *mod, GOutputStream* ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Your model is unbounded\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printError(const void *mod, GOutputStream* ostream){
  gsize bw;
  GError *error = NULL;

  if (!g_output_stream_printf(ostream, &bw, NULL, &error,
        "Something went wrong\n")) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printOptimal(const void *mod, GOutputStream* ostream){
  gsize bw;
  GError *error = NULL;

  const double objectiveVal = Highs_getObjectiveValue(mod);

  if (!g_output_stream_printf(ostream, &bw, NULL, &error, 
          "Objective value:\t\t%lf\nTotal Variables:\t\t\t%d\nTotal Constraints:\t\t%d\n\nVariable\tValue\t\t\tReduced Cost\n",
          objectiveVal,
          numCol, numRow)) {
      g_printerr("Error writing to file: %s\n", error->message);
      g_clear_error(&error);
  }
}

void
printSolToFile(const void *mod, GOutputStream* ostream) {
  char text[kHighsMaximumStringLength];
  //double col_value[numCol], row_value[numRow], col_dual[numCol], row_dual[numRow];// weird bug on free => would segfault => stream_printf async?
  HighsInt status = Highs_getModelStatus(mod);

  //Highs_getSolution(mod, col_value, col_dual, row_value, row_dual);

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

//#endif
