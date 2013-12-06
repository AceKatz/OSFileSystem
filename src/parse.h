
int init_parse(char* fname, char* path){
  FILE* fp = fopen(fname, "r");
  struct sf_root *sf;
  sf = sf_tree_init(path);
  char line [256];
  char user[16];
  char hash[32];
  int dsc;
  int dcc;
  int dmc;
  int warn;
  int expire;
  int dse;
  
  while(fgets(&line, sizeof(line), fp)!= NULL){
    strcpy(user, strtok(line, ":"));
    if(!strcmp(user, "")){
      printf("Parse error, username not found\n");
      fclose(fp);
      return -1;
    }
    sf_tree_add_user(sf, user);
    
    strcpy(hash, strtok(NULL, ":"));
    if(!strcmp(user, "")){
      printf("Parse error, no password hash found\n");
      fclose(fp);
      return -2;
    }
    update_hash(sf, user, hash);
    
    dsc = atoi(strtok(NULL, ":"));
    update_daysSinceChanged(sf, user, dsc);
    
    dcc = atoi(strtok(NULL, ":"));
    updaye_daysUntilCanChange;
    
    
  }
  fclose (fp);
}


