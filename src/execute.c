#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "utilities.h"
#include "execute.h"

#define READ 0
#define WRITE 1

/*
 * Funzione per che legge da buf il comando immesso dall'utente per 
 * stamparlo nei file di log.
 * str_cmd è la stringa che poi verrà stampata.
 * index è l'ultimo indice usato nell'array di comandi (vedere come
 * questo viene implementato in comandi.c), pertanto il numero di 
 * comandi è index+1.
 */
void stampa_cmd(char *str_cmd, struct command *buf, int index){
	char *stringa[1024];
	int num_cmd=index+1; //Numero di comandi immessi dall'utente
	
	int k = 0; //indice per scorrere gli args
	
	strcpy(stringa,"\0");
	
	for(int i=0; i<num_cmd; i++){ //scorre l'array dei comandi
		
		while(buf[i].args[k]!=NULL){
			
			strcat(stringa, buf[i].args[k]);
			strcat(stringa, " ");
			k++;
			
		}
		/*
		 * Se c'è più di un comando, stampa il carattere di piping
		 */
		if(i!=num_cmd-1){
			strcat(stringa, "| ");
		}
		k=0;
		
	}
	
	strcpy(str_cmd,stringa);
}

/*
 * Funzione per l'esecuzione di un comando singolo.
 * Fa uso di due pipe per la stampa su file di log del risultato
 * dell'esecuzione o dell'eventuale errore.
 * Una terza pipe (sul file descriptor tm) è utilizzata per stampare
 * nei file di log la data e l'ora, tramite la chiamata date.
 * Parametri:
 * buf	:	L'array con i comandi (in questo caso con uno solo)
 * outputFile	:	il file descriptor per il file di log
 * errorFile	:	il file descriptor per il file di log degli errori
 */
void esegui(struct command *buf, int outputFile, int errorFile, int code) {

	char message[1024]; //output
	char errmes[1024]; //errore
	char datemes[1024]; //data e ora
	int dbytes; //dimensione effettiva datemes
  
	int fd[2]; //file descriptor per esecuzione senza errori
	int ep[2]; //file descriptor per catch errori
	int tm[2]; //fd per la data
	
	int processo;
	
	char *str_cmd = malloc(1024*sizeof(char));
	stampa_cmd(str_cmd,buf,0);

// INIZIALIZZAZIONE DELLE PIPES
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
	
	if(pid==0){ //processo figlio
		processo=getppid();
		dup2(fd[1], STDOUT_FILENO); //redirezionamento output
		dup2(ep[1], STDERR_FILENO); //redirezionamento errore
		close(fd[0]);
		close(fd[1]);
		if(execvp(buf[0].args[0], buf[0].args) == -1){
			perror("Errore");
		}
		exit(1);
	} else if(pid>0){
		//per aggiungere la data nell'output
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
		// chiusura dei file descriptors
		close(fd[1]);
		close(ep[1]);
		// lettura dei filedescriptor per decidere se stampare sul file
		// di log "normale" o in quello degli errori
		int nbytes = read(fd[0], message, sizeof(message));
		int ebytes = read(ep[0], errmes, sizeof(errmes));
		if(nbytes != 0 && ebytes==0){

			stampa_file(outputFile, str_cmd, dbytes, nbytes, datemes, message, 0, code);

		} else if(ebytes!=0 && nbytes==0){

			stampa_file(errorFile, str_cmd, dbytes, ebytes, datemes, errmes, -1, code);
		}
	}
	free(str_cmd);
}

/*
 * Funzione per l'esecuzione di comandi in pipe.
 * Fa uso di due pipe per la stampa su file di log del risultato
 * dell'esecuzione, una pipe per il redirezionamento dell'errore,
 * mentre una quarta pipe (sul file descriptor tm) è utilizzata per 
 * stampare nei file di log la data e l'ora, tramite la chiamata date.
 * Parametri:
 * buf			:	L'array con i comandi
 * index		:	l'indice dell'ultimo comando nell'array
 * outputFile	:	il file descriptor per il file di log
 * errorFile	:	il file descriptor per il file di log degli errori
 */
void pipeHandler(struct command *buf, int index, int outputFile, int errorFile, int code){
	
	// File descriptors
	int fd[2]; // pos. 0 output, pos. 1 input
	int fd2[2];
	int ep[2];
	int tm[2];
	
	int num_com = index+1; //numero di comandi
	
	char datemes[1024]; //stringa data
	char errmes[1024]; //stringa errore
	char *message[1024]; //stringa output
	char *str_cmd = malloc(1024*sizeof(char)); //stringa del comando
	stampa_cmd(str_cmd,buf,index);
	
//////////////////////////////////////////
//	GESTIONE DATA E ORA PER OUTPUT
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
///////////////////////////////////////////

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
		
		if (i % 2 != 0){ // iterazioni dispari
			if(pipe(fd) == -1){
				perror("pipe");
				exit(1);
			}
		}else{ // iterazioni pari
			if(pipe(fd2) == -1){
				perror("pipe");
				exit(1);
			}
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
		} else if(i == num_com - 1){ //stampa a video e sui file di log quando arriva all'ultimo comando
		// i file descriptors vengono usati alternativamente, quindi bisogna distinguere se i comandi erano 
		// in quantità pari o dispari
			if (num_com % 2 != 0){					
				close(fd[0]);
				close(fd2[1]);
				close(ep[1]);
		// lettura dei filedescriptor per decidere se stampare sul file
		// di log "normale" o in quello degli errori
				int nbytes = read(fd2[0], message, sizeof(message));
				int ebytes = read(ep[0], errmes, sizeof(errmes));
				int dbytes = read(tm[0], datemes,sizeof(datemes));
				if(nbytes != 0 && ebytes==0){

					stampa_file(outputFile, str_cmd, dbytes, nbytes, datemes, message, 0, code);
					
				} else if(ebytes!=0 && nbytes==0){

					stampa_file(errorFile, str_cmd, dbytes, ebytes, datemes, errmes, -1, code);
				}
			} else {				
				close(fd2[0]);
				close(fd[1]);
				close(ep[1]);
				int nbytes = read(fd[0], message, sizeof(message));
				int ebytes = read(ep[0], errmes, sizeof(errmes));
				int dbytes = read(tm[0], datemes,sizeof(datemes));
				if(nbytes != 0 && ebytes==0){

					stampa_file(outputFile, str_cmd, dbytes, nbytes, datemes, message, 0, code);

				} else if(ebytes!=0 && nbytes==0){

					stampa_file(errorFile, str_cmd, dbytes, ebytes, datemes, errmes, -1, code);

				}
			}
		} else { // Un comando intermedio non stampa su file
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
	free(str_cmd);
}

/*
 * Funzione per la stampa su file
 * Parametri:
 * outputFile	:	il file in cui scrivere
 * str_cmd		:	il comando immesso dall'utente
 * dbytes		:	la dimensione effettiva della stringa della data
 * nbytes		:	la dimensione effettiva della stringa di output
 * datemes		:	la stringa della data
 * message		:	la stringa di output
 * resultcode	:	codice di ritorno
 */
void stampa_file(int outputFile, char *str_cmd, int dbytes, int nbytes, char datemes[1024], char message[1024], int resultcode, int code){
	dprintf(outputFile, "============================================\n\n");
	dprintf(outputFile, "COMMAND: %s \n",str_cmd);
	dprintf(outputFile,"SHELL PID: %d \n",getppid());
	dprintf(outputFile,"DATE: %.*s \n",dbytes, datemes);
	dprintf(outputFile,"OUTPUT: \n\n%.*s \n", nbytes, message);
	if (code==1){
		dprintf(outputFile,"RETURN CODE: %d\n", resultcode);
	}
	dprintf(outputFile, "============================================\n\n");
	printf("%.*s", nbytes, message);
}