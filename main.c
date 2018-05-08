#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "comandi.h"

int concatena = 0;

char **splitline(char *line){
  int pos=0;
  char **args=malloc(60 * sizeof(char*));
  char *word;

  if(!args){
    fprintf(stderr, "allocation error\n");
    exit(EXIT_FAILURE);
  }
  word = strtok(line, " ");
  while(word!= NULL){
    args[pos]=word;
    pos++;
    word=strtok(NULL, " ");
  }
  args[pos]=NULL;

  if(pos>0){
    if(strcmp(args[pos-1],"&") == 0){
      concatena = 1;
      args[pos-1] = NULL;
      //printf("concatena\n");
    } else {
      concatena = 0;
      //printf("non concatena\n");
    }
  }

  return args;
}

char *readline(void){
  int pos=0;
  char *buffer=malloc(64 * sizeof(char));
  int l;

  while(1){
    l=getchar();
    if(l==EOF || l=='\n'){
      buffer[pos]='\0';
      break;
    } else {
      buffer[pos]=l;
    }
    pos++;
  }

  return buffer;
}

int launch(char **args)
{
    pid_t pid;

    pid = fork();

    if(pid < 0){ //error occurred
      fprintf(stderr, "Fork Failed");
    } else if (pid == 0){ //child process
      if(execvp(args[0], args) == -1) perror("error");
      exit(EXIT_FAILURE);
    } else { //parent process
      /* parent will wait for the child to complete */
      if(concatena==0){
        wait(NULL);
      }
      //printf("Child Complete");
    }
    return 1;
}

int main(void){

  char **args = malloc(32 * sizeof(char));
  char *buf = malloc(128 * sizeof(char));
  char command[32];
  int status=0;
  char *m_cwd = malloc(128 * sizeof(char));

  //do{
  while(1){

    getcwd(m_cwd,128);
    printf("LAURA_SHELL @ ");
    printf(m_cwd); //mostra la current working directory
    printf(" > ");

    buf=readline();
    if (strcmp(buf,"exit")==0) {
      break;
    }
    if(buf[0]=='\0'){
      continue;
    }
    args=splitline(buf);
    if(strcmp(args[0],"help")==0){
      status=aiuto();
    } else if(strcmp(args[0],"cd")==0){  //comando cd
      status=funz_cd(args);
    } else {                //tutto il resto
      status=launch(args);
    }

  }//while(status);


  return 0;
}
