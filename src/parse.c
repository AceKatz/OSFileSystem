#include <stdio.h>
#include "shadowTree.h"
#include "parse.h"

int main(){
  struct sf_root *sf;
  sf = init_parse("shadow.test", "/shadow/");
  printf("First user: %s\n", sf->head->username);
  printf("Second user: %s\n", sf->head->next->username);
  sf_deparse(sf, "shadow.out");
}
