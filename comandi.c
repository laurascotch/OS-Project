#include "comandi.h"
#include <stdio.h>
#include <string.h>

void aiuto(){
  printf("Custom shell\n");
  printf("Digita il comando e gli argomenti e premi INVIO\n");
  printf("digita help per la guida\n");
  printf("digita & alla fine del comando per eseguire il programma in background e continuare a usare il terminale\n");

  return 1;
}

void funz_cd(char **args){
    if(chdir(args[1]) != 0){
        perror("impossibile eseguire l'operazione");
    }
    return 1;
}
