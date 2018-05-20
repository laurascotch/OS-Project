#include <stdio.h>
#include <stdlib.h>

int errno;
FILE *logfile;
FILE *errorfile;

void print_error(char* error_message){
    printf(error_message);
    fprintf(errorfile, error_message);
}

void print_log(char* message){
    printf("log: %s", message);
    fprintf(logfile,message);
}

int main( int argc, char *argv[] )
{
    FILE *fp;    
    logfile = fopen("file.txt", "wb");//todo: nomi file come input da shell all'avvio
    errorfile = fopen("error.txt","wb");
    if (logfile == NULL)
    {
        print_error("Error opening file!");
        exit(1);
    }
    
    char path[1035];
    char * command = malloc(128 * sizeof(char));
    command = "ls  ";
    
    /* Open the command for reading. */
    //2>&1 serve per reindirizzare lo stderr per far si che non stampi direttamente su shell quando c'è un errore ma metta tutto nel file fp
    fp = popen("ls -l 2>&1", "r");
    if (fp == NULL) {
        print_error("Failed to run command\n");
        exit(1);
    }
    
    //legge tutto il file fp e mette dentro path
    while (fgets(path, 2460, fp) != NULL) {}
    
    //pclose ritorna un errno e se è 0 vuol dire che popen non ha avuto errori nel lancio della shell se è diverso da 0 gestisci l'errore 
    if(pclose(fp) != 0){
        print_error(path);
    }
    else{
        print_log(path);
    }

    fclose(logfile);
    fclose(errorfile);
    return 0;
}
