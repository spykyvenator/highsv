#include "interfaces/highs_c_api.h"
#include <stdio.h>

int
main(void){
  void *model;
  char *text;

  model = Highs_create();
  HighsInt i =  Highs_getNumOptions(model);
  printf("numOptiosn: %d",i);
  for (HighsInt j = 0; j < i; j++){
    Highs_getOptionName (model, j, &text);
    puts(text);
  }
Highs_destroy(model);
}
