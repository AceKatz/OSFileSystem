#include <stdio.h>
#include "shadowTree.h"
#include "parse.h"
#include "md5.h"

int main(){
  struct sf_root *sf;
  sf = init_parse("shadow.out", "/shadow/");
  printf("First user: %s\n", sf->head->username);
  printf("Second user: %s\n", sf->head->next->username);
  update_hash(sf, "root", hashword("password2"));
  printf("Root's hash: %s\n", sf->head->hash);
  sf_deparse(sf, "shadow.out", 1);
  
}
