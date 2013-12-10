
struct sf_root* init_parse(char* fname, char* path){
  FILE* fin;
  struct sf_root *sf;


  char hash[16];
  char *token;
  int dsc;
  int dcc;
  int dmc;
  int warn;
  int expire;
  int dse;
  char user[16];
  
  fin = fopen(fname, "r");  
  sf = sf_tree_init(path);
  
  char line [256];
  while(fgets(&line, sizeof(line), fin)!= NULL){
  
    strcpy(user, strtok(line, ":"));
    if(!strcmp(user, "")){
      printf("Parse error, username not found\n");
      fclose(fin);
      return sf;
    }
    sf_tree_add_user(sf, user);
    
    strcpy(hash, strtok(NULL, ":"));
    if(!strcmp(user, "")){
      printf("Parse error, no password hash found\n");
      fclose(fin);
      return sf;
    }
    update_hash(sf, user, hash);
  
    token = strtok(NULL, ":");
    dsc = (token==NULL)? 0: atoi(token);
    update_daysSinceChanged(sf, user, dsc);

    token = strtok(NULL, ":");
    dcc = (token==NULL)? 0: atoi(token);
    update_daysUntilCanChange(sf, user, dcc);
    
    token = strtok(NULL, ":");
    dmc = (token==NULL)? 0 : atoi(token);
    update_daysUntilMustChange(sf, user, dmc);
    
    token = strtok(NULL, ":");
    warn = (token==NULL)? 0 : atoi(token); 
    update_daysBeforeWarning(sf, user, warn);
    
    token = strtok(NULL, ":");
    expire = (token==NULL)? 0 : atoi(token);
    update_daysUntilExpiration(sf, user, expire);
    

     token = strtok(NULL, ":");
     dse = (token==NULL)? 0 : atoi(token);
     update_daysSinceDeactivation(sf, user, dse);
     
     
  }
  return sf;
  fclose (fin);
}

int sf_deparse(struct sf_root *sf, char* filename, int frees){
  FILE* fout;
  struct user *current;
  int writeInt;
  char* buf;
  
  fout = fopen(filename, "w");
  
  current = sf->head;
  while(current!=NULL){
    fprintf(fout, "%s:%s:%d:", current->username, current->hash, current->dsc);
    
    if(current->dcc == 0) fprintf(fout, ":");
    else fprintf(fout, "%d:", current->dcc);

    if(current->dmc == 0) fprintf(fout, ":");
    else fprintf(fout, "%d:", current->dmc);
    
    if(current->dw == 0) fprintf(fout, ":");
    else fprintf(fout, "%d:", current->dw);
    
    if(current->de == 0) fprintf(fout, ":");
    else fprintf(fout, "%d:", current->de);
    
    if(current->dd == 0) fprintf(fout, ":");
    else fprintf(fout, "%d:", current->dd);
    
    if( current->reserved == NULL ) fprintf(fout, "\n");
    else fprintf(fout, "%s\n", current->reserved);
    
    current = current->next;
  }
  if(frees){
    sf_tree_destroy(sf);
  }
  fclose(fout);
}
