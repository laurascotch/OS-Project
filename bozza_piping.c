#include <stdio.h>

#define READ 0
#define WRITE 1

void splitargs(char **args, char **arg1, char **arg2){

  char *word = malloc(32 * sizeof(char));
  int pos=0;
  int p=0;
  int pip=0;
  //word = args[pos]; //la prende solo per farla essere diversa da null ed entrare nello while
  while(args[pos]!=NULL){
    word = args[pos]; //qui inizia effettivamente
    printf(word);
    if(strcmp(word,"|")==0){
      pip=1;
      pos++;
      word = args[pos];
      arg1[pos]=NULL;
    }
    if(pip==0){
      arg1[pos]=word;
    } else {
      arg2[p]=word;
      p++;
    }
    pos++;
  }
  arg2[p+1]=NULL;
}

/* redirige lo stdout di cmd1 sullo stdin di cmd2 */
int fai_pipe(char **args) {

  char **arg1 = malloc(64 * sizeof(char));
  char **arg2 = malloc(64 * sizeof(char));
  splitargs(args, arg1, arg2);

  int fd[2];
  pipe(fd); /* Create an unamed pipe */
  if(fork () != 0) { /* Parent, writer */
    close(fd[READ]); /* Close unused end */
    dup2(fd[WRITE], 1); /* Duplicate used end to stdout */
    close(fd[WRITE]); /* Close original used end */
    execvp(arg1[0], arg1); /* Execute writer program */
    perror("connect"); /* Should never execute */
  } else { /* Child, reader */
    close(fd[WRITE]); /* Close unused end */
    dup2(fd[READ], 0); /* Duplicate used end to stdin */
    close(fd[READ]); /* Close original used end */
    execvp(arg2[0], arg2); /* Execute reader program */
    perror("connect"); /* Should never execute */
  }

return 1;
}
