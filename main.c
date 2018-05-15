#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "comandi.h"

void esegui(char * protocomm){
    FILE *fp;
    FILE *f = fopen("file.txt", "a");
    FILE *e = fopen("error.txt","a");
    if (f == NULL||e==NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    char path[1035];
    //char * command = malloc(128 * sizeof(char));
    char redirect = " 2>&l";

    char *command = malloc(strlen(protocomm)+strlen(redirect)+1);
    command[0]='\0';

    strcat(command, protocomm);
    strcat(command, redirect);

    /* Open the command for reading. */
    //2>&1 serve per reindirizzare lo stderr per far si che non stampi direttamente su shell quando c'è un errore ma metta tutto nel file fp
    fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to run command\n" );
        exit(1);
    }

    //legge tutto il file fp e mette dentro path
    while (fgets(path, 2460, fp) != NULL);

    int a=pclose(fp);
    //pclose ritorna un errno e se è 0 vuol dire che popen non ha avuto errori nel lancio della shell se è diverso da 0 gestisci l'errore
    if(a!=0){
        //printf("prova");
        printf("%s",path);
        fprintf(e,"%s",path);
    }
    else{
        printf("%s",path);
        fprintf(f,"%s",path);
    }
    fclose(f);
    fclose(e);
    return 0;
}

int main(void){
    
    char **args = malloc(32 * sizeof(char));
    char *buf = malloc(128 * sizeof(char));
    char *buf2 = malloc(128 * sizeof(char));
    char *buftext = malloc(128 * sizeof(char));
    char command[128];
    int status=0;
    char *m_cwd = malloc(128 * sizeof(char));
    
    //do{
    while(1){
        
        getcwd(m_cwd,128);
        printf("CUSTOM SHELL @ ");
        printf(m_cwd); //mostra la current working directory
        printf(" > ");

        scanf("%s", command);

        if (strcmp(command,"exit")==0) {
            exit(0);
        }
        if(command[0]=='\0'){
            continue;
        }
        if(strcmp(command,"help")==0){
            aiuto();
        }
        
        else if(strcmp(command,"cd")==0){
            funz_cd(args);
        }
        else {
            esegui(command);
        }
        
    }
}
