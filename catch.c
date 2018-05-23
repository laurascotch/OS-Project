#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "comandi.h"

#define READ 0
#define WRITE 1

void esegui(struct command *buf) {

  char message[1024];
  int fd[2];

  FILE *f = fopen("file.txt", "a+");
  FILE *e = fopen("error.txt","a+");

  if (pipe(fd) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t pid = fork();

  if(pid==0){
    dup2 (fd[1], STDOUT_FILENO);
    close(fd[0]);
    close(fd[1]);
    execvp(buf[0].args[0], buf[0].args);
    perror("execvp");
  } else if(pid>0){
    close(fd[1]);
    int nbytes = read(fd[0], message, sizeof(message));
    fprintf(f, "Comando: %s \n",buf[0].args[0]);
    fprintf(f,"Output: %.*s \n", nbytes, message);
    fprintf(f, "--------------------------------------------\n\n");
    printf("%.*s", nbytes, message);
    //qui si può mettere una wait...
  }
}

void pipeHandler(struct command *buf, int index){
	
	// File descriptors
	int fd[2]; // pos. 0 output, pos. 1 input
	int fd2[2];
	
	int num_com = index+1;
	
	char *command[256];
	char message[1024];
	int nbytes;
	
	pid_t pid;
	
	int err = -1;
	
	// contatore per il loop
	int i = 0;
	
	// Loop del metodo. Per ogni comando tra un carattere di piping
	// e l'altro, verranno utilizzate le pipe appropriate, quindi 
	// verrà eseguito il comando
	
	while (i<num_com){	
		
		// In base all'iterazione in cui ci troviamo, usiamo file 
		// descriptors (e pipe) diverse. In questo modo, una pipe
		// viene riutilizzata ogni due iterazioni, permettendoci di 
		// connettere gli input e gli output di due comandi diversi
		
		if (i % 2 != 0){
			pipe(fd); 	// iterazioni dispari
		}else{
			pipe(fd2); // iterazioni pari
		}
		
		pid=fork();
		
		if(pid==-1){
			if (i != num_com - 1){ 	// se non è l'ultimo
				if (i % 2 != 0){
					close(fd[1]); 	// iterazioni dispari
				}else{
					close(fd2[1]); // iterazioni pari
				} 
			}			
			perror("Fork\n");
			return;
		}
		if(pid==0){	// processo figlio
			if (i == 0){ // se è il primo

				dup2(fd2[1], STDOUT_FILENO);
			}
			
			// Se siamo all'ultimo comando, sostituiamo lo standard
			// input per una pipe o l'altra, in base al numero pari
			// o dispari dell'iterazione. Lo standard output è
			// intoccato perché vogliamo vedere l'output a terminale
			
			else if (i == num_com - 1){ // ultimo comando
				if (num_com % 2 != 0){ // per un numero dispari di comandi totali
					dup2(fd[0],STDIN_FILENO);
				}else{ // numero pari di comandi totali
					dup2(fd2[0],STDIN_FILENO);
				}
				
			// Se siamo in un comando "in mezzo" dobbiamo utilizzare
			// due pipe, una per l'input e una per l'output. La 
			// posizione è importante per scegliere quale file 
			// descriptor corrisponde a ogni input/output
				
			} else { 
				if (i % 2 != 0){	// iterazione dispari
					dup2(fd2[0],STDIN_FILENO); 
					dup2(fd[1],STDOUT_FILENO);
				}else{ 				// iterazione pari
					dup2(fd[0],STDIN_FILENO); 
					dup2(fd2[1],STDOUT_FILENO);					
				} 
			}
			
			execvp(buf[i].args[0], buf[i].args);
			perror("Error execvp");	// non lo esegue se va tutto bene
		}
		// chiusura dei file descriptor nel processo padre.
		if (i == 0){
			close(fd2[1]);
		} else if(i == num_com - 1){
			if (num_com % 2 != 0){					
				close(fd[0]);
					close(fd[1]);
			} else {					
				close(fd2[0]);
					close(fd2[0]);
			}
		} else {
			if(i % 2 != 0){					
				close(fd2[0]);
				close(fd[1]);
			} else {					
				close(fd[0]);
				close(fd2[1]);
			}
			waitpid(pid,NULL,0);
		}		
		i=i+1;	
	}
}

