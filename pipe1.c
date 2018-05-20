#include <stdio.h>
#include <stdlib.h>

void print_error(char* error_message){
    perror(error_message);
}

void print_log(char* message){
    printf("test log %s", message);
}

int errno;
FILE *logfile;
FILE *errorfile;

int main( int argc, char *argv[] )
{

    FILE *fp;    
    FILE *logfile = fopen("file.txt", "w");//todo: nomi file come input da shell all'avvio
    FILE *errorfile = fopen("error.txt","w");
    if (logfile == NULL)
    {
        printf("Error opening file!\n");
        exit(1);
    }
    
    char path[1035];
    char * command = malloc(128 * sizeof(char));
    command = "ls  ";
    
    /* Open the command for reading. */
    //2>&1 serve per reindirizzare lo stderr per far si che non stampi direttamente su shell quando c'è un errore ma metta tutto nel file fp
    fp = popen("ls -l | wcsas 2>&1", "r");
    if (fp == NULL) {
        print_error("Failed to run command\n");
        exit(1);
    }
    
    //legge tutto il file fp e mette dentro path
    while (fgets(path, 2460, fp) != NULL) {}
    int a=pclose(fp);
    if(errno!=0){

    }


    //pclose ritorna un errno e se è 0 vuol dire che popen non ha avuto errori nel lancio della shell se è diverso da 0 gestisci l'errore 
        if(a!=0){
        //printf("prova");
        printf("%s",path);
        fprintf(errorfile,"%s",path);
    }
    else{
        printf("%s",path);
        fprintf(logfile,"%s",path);
    }

    fclose(logfile);
    fclose(errorfile);
    return 0;
}
