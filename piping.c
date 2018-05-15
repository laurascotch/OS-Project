#include <stdio.h>
#include <stdlib.h>
#include "piping.h"


int fai_piping(char *args) {

    FILE *fp;

    FILE *f = fopen("file.txt", "w");
    FILE *e = fopen("error.txt","w");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }

    char path[1035];
    char * command = malloc(128 * sizeof(char));
    command = "ls | ";
    char * arg_popen;
    char * str_bella = malloc(32 * sizeof(char));
    str_bella = " 2>&1";

    if((arg_popen = malloc(strlen(args)+strlen(str_bella)+1)) != NULL){
       arg_popen[0] = '\0';   // ensures the memory is an empty string
       strcat(arg_popen, args);
       strcat(arg_popen, str_bella);
   } else {
       fprintf(stderr,"malloc failed!\n");
       // exit
   }
   //printf("%s",new_str);

    /* Open the command for reading. */
    //2>&1 serve per reindirizzare lo stderr per far si che non stampi direttamente su shell quando c'è un errore ma metta tutto nel file fp
    //fp = popen("ls -l | wc 2>&1", "r");
    fp = popen(arg_popen, "r");
    if (fp == NULL) {
        perror("Failed to run command\n" );
        exit(1);
    }

    //legge tutto il file fp e mette dentro path
    while (fgets(path, 2460, fp) != NULL) {}

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
