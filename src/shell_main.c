#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "utilities.h"
#include "execute.h"

/*
 * Metodo che invoca le funzione appropriate per eseguire i comandi 
 * inseriti dall'utente. Controlla se è necessario usare il piping.
 * Parametri:
 * buf	:	Un array di comandi di tipo command, già definito in 
 * 			comandi.h e comandi.c
 * index:	L'ultimo indice utile dell'array, index+1 è il numero di
 * 			comandi da eseguire in totale
 * oF	:	File descriptor per il file di log
 * eF	:	File descriptor per il file di log degli errori
 */
int launch(struct command *buf, int index, int oF, int eF) {
	
	if(index==0){
		esegui(buf, oF, eF);
	} else {
		pipeHandler(buf, index, oF, eF);
	}

return 1;
}

/*
 * Metodo main della shell, accetta gli argomenti da linea di comando
 * per l'inizializzazione dei file di log.
 */
int main(int argc, char *argv[]){

	struct command *buf = malloc(128 * sizeof(char)); //Array che conterrà i comandi da eseguire
	int status=0;
	int *index=0; //Ultimo indice utile dell'array
	char *m_cwd = malloc(128 * sizeof(char)); //Stringa della current working directory
	
	char *outPath=malloc(128*sizeof(char)); //percorso del file di log
	char *errPath=malloc(128*sizeof(char)); //percorso del file di log degli errori

	int oF; //file descriptor outfile
	int eF; //file descriptor errfile
	
	//chiamate a funzioni in comandi.c
	argCheck(argc, argv, outPath, errPath);
	fileOpen(outPath, errPath, &oF, &eF);

	//"pulisce" il terminale alla prima chiamata
	system("clear");

	// Loop principale del programma, accetta i comandi immessi
	// dall'utente
	while(1){

		// INTESTAZIONE
		getcwd(m_cwd,128);
		printf("\033[1;36m");
		printf("CUSTOM SHELL @ ");
		printf("\x1B[35m");
		printf(m_cwd); //mostra la current working directory
		printf(" > ");
		printf("\033[0m");

		buf=readline(&index);

		if (strcmp(buf[0].args[0],"exit")==0) { // supponiamo che l'utente scriva solo "exit" per uscire
			break;
		}
		if (strcmp(buf[0].args[0],"\0")==0) { //a capo a vuoto
			continue;
		}
	
		if(strcmp(buf[0].args[0],"help")==0){ // guida
			status=aiuto();
		} else if(strcmp(buf[0].args[0],"cd")==0){ // comando cd
			status=funz_cd(buf);
		} else { //tutto il resto
			status=launch(buf, index, oF, eF);
		}
	}
	close(oF);
	close(eF);

	free(outPath);
	free(errPath);
	free(m_cwd);
	free(buf);

return 0;
}
