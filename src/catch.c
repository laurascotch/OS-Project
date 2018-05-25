#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "comandi.h"
#include "catch.h"

#define READ 0
#define WRITE 1

void stampa_cmd(char *str_cmd, struct command *buf, int index){
	char *stringa[1024];
	int num_cmd=index+1;
	int k = 0;
	
	strcpy(stringa,"\0");
	
	for(int i=0; i<num_cmd; i++){
		
		while(buf[i].args[k]!=NULL){
			
			strcat(stringa, buf[i].args[k]);
			strcat(stringa, " ");
			k++;
			
		}
		if(i!=num_cmd-1){
			strcat(stringa, "| ");
		}
		k=0;
		
	}
	
	strcpy(str_cmd,stringa);
}


void esegui(struct command *buf, int oF, int eF) {

	char message[1024];
	char errmes[1024];
	char datemes[1024];
	int dbytes;
  
	int fd[2];
	int ep[2];
	int tm[2];
	
	int processo;
	
	char *str_cmd = malloc(1024*sizeof(char));
	stampa_cmd(str_cmd,buf,0);

	if (pipe(ep) == -1) {
		perror("pipe");
		exit(1);
	}

	if (pipe(fd) == -1) {
		perror("pipe");
		exit(1);
	}
	
	if (pipe(tm) == -1) {
		perror("pipe");
		exit(1);
	}

	pid_t pid = fork();
	
	if(pid==0){
		processo=getppid();
		dup2(fd[1], STDOUT_FILENO);
		dup2(ep[1], STDERR_FILENO);
		close(fd[0]);
		close(fd[1]);
		if(execvp(buf[0].args[0], buf[0].args) == -1){
			perror("Errore");
		}
		exit(1);
	} else if(pid>0){
		pid_t pidtime = fork();
		if(pidtime==0){
			dup2(tm[1],STDOUT_FILENO);    
			close(tm[0]);
			execlp("date","date",NULL);
			exit(1);
		} else if(pidtime>0){
			close(tm[1]);
			dbytes = read(tm[0], datemes,sizeof(datemes));  
		}
		close(fd[1]);
		close(ep[1]);
		int nbytes = read(fd[0], message, sizeof(message));
		int ebytes = read(ep[0], errmes, sizeof(errmes));
		if(nbytes != 0 && ebytes==0){
			dprintf(oF, "============================================\n\n");
			dprintf(oF, "Comando: %s \n",str_cmd);
			dprintf(oF,"Shell pid: %d \n",getppid());
			dprintf(oF,"Data: %s \n",datemes);
			dprintf(oF,"Output: \n\n%.*s \n", nbytes, message);
			dprintf(oF, "============================================\n\n");
			printf("%.*s", nbytes, message);
		} else if(ebytes!=0 && nbytes==0){
			dprintf(eF, "============================================\n\n");
			dprintf(eF, "Comando: %s \n",str_cmd);
			dprintf(eF,"Shell pid: %d \n",getppid());
			dprintf(eF,"Data: %s \n",datemes);
			dprintf(eF,"%.*s \n", ebytes, errmes);
			dprintf(eF, "============================================\n\n");
			printf("%.*s", ebytes, errmes);
		}
		
		//qui si può mettere una wait...
	}
}

void pipeHandler(struct command *buf, int index, FILE *oF, FILE *eF){
	
	// File descriptors
	int fd[2]; // pos. 0 output, pos. 1 input
	int fd2[2];
	int ep[2];
	int tm[2];
	
	int num_com = index+1;
	
	char datemes[1024];
	char errmes[1024];
	char *message[1024];
	char *str_cmd = malloc(1024*sizeof(char));
	stampa_cmd(str_cmd,buf,index);
	
	if(pipe(tm) == -1){
		perror("pipe");
		exit(1);
	}
	pid_t pidtime = fork();
	if(pidtime==0){
		dup2(tm[1],STDOUT_FILENO);
		close(tm[0]);
		execlp("date","date",NULL);
		exit(1);
	} else if(pidtime>0){
		close(tm[1]);
	}
	
	pid_t pid;
	
	int processo;
	
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
		
		pipe(ep);
		
		if (i % 2 != 0){
			if(pipe(fd) == -1){
				perror("pipe");
				exit(1);
			}
		//	pipe(fd); 	// iterazioni dispari
		}else{
			if(pipe(fd2) == -1){
				perror("pipe");
				exit(1);
			}
		//	pipe(fd2); // iterazioni pari
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
			
			processo=getppid();
			
			dup2(ep[1], STDERR_FILENO);
			
			if (i == 0){ // se è il primo

				dup2(fd2[1], STDOUT_FILENO);
			
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
			
			if(execvp(buf[i].args[0], buf[i].args) == -1){
				perror("Error execvp");
			}
			exit(1);
			
		}
		// chiusura dei file descriptor nel processo padre.
		if (i == 0){
			close(fd2[1]);
		} else if(i == num_com - 1){
			if (num_com % 2 != 0){					
				close(fd[0]);
				close(fd2[1]);
				close(ep[1]);
				int nbytes = read(fd2[0], message, sizeof(message));
				int ebytes = read(ep[0], errmes, sizeof(errmes));
				int dbytes = read(tm[0], datemes,sizeof(datemes));
				if(nbytes != 0 && ebytes==0){
					dprintf(oF, "============================================\n\n");
					dprintf(oF, "Comando: %s \n",str_cmd);
					dprintf(oF,"Shell pid: %d \n",getppid());
					dprintf(oF,"Data: %s \n",datemes);
					dprintf(oF,"Output: \n\n%.*s \n", nbytes, message);
					dprintf(oF, "============================================\n\n");
					printf("%.*s", nbytes, message);
				} else if(ebytes!=0 && nbytes==0){
					dprintf(eF, "============================================\n\n");
					dprintf(eF, "Comando: %s \n",str_cmd);
					dprintf(eF,"Shell pid: %d \n",getppid());
					dprintf(eF,"Data: %s \n",datemes);
					dprintf(eF,"%.*s \n", ebytes, errmes);
					dprintf(eF, "============================================\n\n");
					printf("%.*s", ebytes, errmes);
				}
			} else {				
				close(fd2[0]);
				close(fd[1]);
				close(ep[1]);
				int nbytes = read(fd[0], message, sizeof(message));
				int ebytes = read(ep[0], errmes, sizeof(errmes));
				int dbytes = read(tm[0], datemes,sizeof(datemes));
				if(nbytes != 0 && ebytes==0){
					dprintf(oF, "============================================\n\n");
					dprintf(oF, "Comando: %s \n",str_cmd);
					dprintf(oF,"Shell pid: %d \n",getppid());
					dprintf(oF,"Data: %s \n",datemes);
					dprintf(oF,"Output: \n\n%.*s \n", nbytes, message);
					dprintf(oF, "============================================\n\n");
					printf("%.*s", nbytes, message);
				} else if(ebytes!=0 && nbytes==0){
					dprintf(eF, "============================================\n\n");
					dprintf(eF, "Comando: %s \n",str_cmd);
					dprintf(eF,"Shell pid: %d \n",getppid());
					dprintf(eF,"Data: %s \n",datemes);
					dprintf(eF,"%.*s \n", ebytes, errmes);
					dprintf(eF, "============================================\n\n");
					printf("%.*s", ebytes, errmes);
				}
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

