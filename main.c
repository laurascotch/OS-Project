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

char * modificabuf(char * line){
    char * new_str ;
    
    char * str2 =  malloc(32 * sizeof(char));
    
    str2 = " 2> error.txt";
    if((new_str = malloc(strlen(line)+strlen(str2)+1)) != NULL){
        new_str[0] = '\0';   // ensures the memory is an empty string
        strcat(new_str,line);
        strcat(new_str,str2);
    } else {
        fprintf(stderr,"malloc failed!\n");
        // exit
    }
    //printf("%s",new_str);
    return new_str;
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

int esegui(char * command){
    FILE *fp;
    FILE *f = fopen("output.txt", "w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    char path[1035];
    
    fp = popen(command, "r");
    
    if (fp == NULL) {
        perror("Failed to run command\n" );
        exit(1);
    }
    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        //il codice funziona con fprintf(stdout) e non funziona con un printf normale non so bene perché 
        fprintf(stdout,"%s", path);
        fprintf(f,"%s",path);

       
        //fprintf(stderr, "printed to stderr\n");
        
    }
    int a=pclose(fp);
    return a;
}

int main(void){
    
    char **args = malloc(32 * sizeof(char));
    char *buf = malloc(128 * sizeof(char));
    char *buf2 = malloc(128 * sizeof(char));
    char *buftext = malloc(128 * sizeof(char));
    char command[32];
    int status=0;
    char *m_cwd = malloc(128 * sizeof(char));
    
    //do{
    while(1){
        
        getcwd(m_cwd,128);
        printf("ASHELLA @ ");
        printf(m_cwd); //mostra la current working directory
        printf(" > ");
        buf=readline();
        
        strcpy(buf2,buf);
        strcpy(buftext,buf);
        args=splitline(buf2);
        buftext = modificabuf(buf);
        
        //printf("%s",args[1]);
        if (strcmp(buf,"exit")==0) {
            break;
        }
        if(buf[0]=='\0'){
            continue;
        }
        if(strcmp(buf,"help")==0){
            status=aiuto();
        }
        
        else if(strcmp(args[0],"cd")==0){
            
            status=funz_cd(args);
        }
        else {
            if(esegui(buftext)!=0){
                perror("errore");
            }
        }
        
    }//while(status);
    
    
    return 0;
}
