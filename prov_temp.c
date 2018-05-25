#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "catch.h"


int main()
{
    FILE *f = fopen("file.txt", "a+");
     char datemes[1024];
    int tm[2];
    if(pipe(tm) == -1){
        perror("pipe");
        exit(1);
    }
    
    if(pidtime==0){
        dup2(tm[1],STDOUT_FILENO);
        
        close(tm[0]);
        execlp("date","date",NULL);
        exit(1);
    }
    
    
}
