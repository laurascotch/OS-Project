#include "comandi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int aiuto(){
  printf("Laura's shell\n");
  printf("Digita il programma e gli argomenti e premi INVIO\n");
  printf("digita help per la guida\n");
  printf("digita & alla fine del comando per eseguire il programma in background e continuare a usare il terminale\n");

  return 1;
}

int funz_cd(struct command *buf){
  if(buf[0].args[1] == NULL){
      fprintf(stderr, "Expected argument to \"cd\"\n");
  }else{
      if(chdir(buf[0].args[1]) != 0){
          perror("impossibile eseguire l'operazione");
      }
  }
  return 1;
}

void argCheck(int argc, char **argv, char *outputPath, char *errPath){
    char *tmp;

    if (argc!=3){
        printf("Missing one or more arguments\n");
        exit(1);
    }

    for (int i=1; i<argc; i++){
        if ((strstr(argv[i], "--outfile")!=NULL)||(strstr(argv[i], "-o")!=NULL)){
            tmp = strchr(argv[i], '=');
            if (tmp!=NULL){
                strncpy(outputPath, argv[i]+(tmp-argv[i]+1), sizeof(outputPath));
                if (outputPath[0]=='\0'){
                    printf("Some error occured: filepath(s) not specified\nCorrect usage \"./shell -o=\"<path-to-file>\" -e=\"<path-to-file>\"\n");
                    exit(1);
                }
            } else {
                printf("Some error occured: filepath(s) not specified\nCorrect usage \"./shell -o=\"<path-to-file>\" -e=\"<path-to-file>\"\n");
                exit(1);
            }
        } else if ((strstr(argv[i], "--errfile")!=NULL)||(strstr(argv[i], "-e")!=NULL)){
            tmp = strchr(argv[i], '=');
            if (tmp!=NULL){
                strncpy(errPath, argv[i]+(tmp-argv[i]+1), sizeof(errPath));
                if (errPath[0]=='\0'){
                    printf("Some error occured: filepath(s) not specified\nCorrect usage \"./shell -o=\"<path-to-file>\" -e=\"<path-to-file>\"\n");
                    exit(1);
                }
            } else {
                printf("Some error occured: filepath(s) not specified\nCorrect usage \"./shell -o=\"<path-to-file>\" -e=\"<path-to-file>\"\n");
                exit(1);
            }
        } else {
            printf("Some error occured: filepath(s) not specified\nCorrect usage \"./shell -o=\"<path-to-file>\" -e=\"<path-to-file>\"\n");
            exit(1);
        }
    }
}

void fileOpen(char *outputPath, char *errPath, int *oF, int *eF){
    *oF=open(outputPath, O_WRONLY|O_APPEND|O_CREAT, 0777);
    *eF=open(errPath, O_WRONLY|O_APPEND|O_CREAT, 0777);

    if (*oF<0){
        printf("Error opening \"%s\". Please check the path and retry\n", outputPath);
        perror(""+errno);
        exit(1);
    }
    if (*eF<0){
        printf("Error opening \"%s\". Please check the path and retry\n", errPath);
        perror(""+errno);
        exit(1);
    }
}

struct command *readline(int *index){
    int pos=0, i=0, j=0;
    struct command *commands=malloc(5*sizeof(struct command));
    char *buffer=malloc(32*sizeof(char));
    char l;

    for (int ind=0; ind<5; ind++){
        commands[ind].args=malloc(32*sizeof(char));
    }

    l=getchar();

    while(l!=EOF && l!='\n'){
        if(l==' '){
            buffer[pos]='\0';
            if(buffer[0]!='\0'){
                commands[i].args[j]=malloc(strlen(buffer)*sizeof(char));
                strcpy(commands[i].args[j], buffer);
                j++;
            }
            pos=0;
            buffer[pos]='\0';
            
        } else if(l=='|'){
            buffer[pos]='\0';
            if (buffer[0]!='\0'){
                commands[i].args[j]=malloc(strlen(buffer)*sizeof(char));
                strcpy(commands[i].args[j], buffer);
                commands[i].args[j+1] = NULL;
            }
            i++;
            j=0;
            pos=0;
        } else {
            buffer[pos]=l;
            pos++;
        }
        l=getchar();
    }
    buffer[pos]='\0';
    commands[i].args[j]=malloc(strlen(buffer)*sizeof(char));
    strcpy(commands[i].args[j], buffer);
    *index=i;
    return  commands;
}
