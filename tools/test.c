#include "interfaces/highs_c_api.h"
#include <stdio.h>

int
main(void){
  void *model;
  char *text;

  model = Highs_create();
  HighsInt i =  Highs_getNumOptions(model);
HighsInt type;
  printf("numOptiosn: %d\n",i);
  for (HighsInt j = 0; j < i; j++){
    Highs_getOptionName (model, j, &text);
    Highs_getOptionType(model, text, &type);
    printf("option: %d: %s type: %d \n",j,text, type);
  }
Highs_destroy(model);
}
