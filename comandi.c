#include "comandi.h"
#include <stdio.h>
#include <string.h>

int aiuto(){
  printf("Laura's shell\n");
  printf("Digita il programma e gli argomenti e premi INVIO\n");
  printf("digita help per la guida\n");
  printf("digita & alla fine del comando per eseguire il programma in background e continuare a usare il terminale\n");

  return 1;
}

int funz_cd(char **args){
  if(args[1] == NULL){
      fprintf(stderr, "Expected argument to \"cd\"\n");
  }else{
      if(chdir(args[1]) != 0){
          perror("impossibile eseguire l'operazione");
      }
  }
  return 1;
}
