#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "comandi.h"
#include "catch.h"


int launch(struct command *buf, int index, int oF, int eF) {
	
	if(index==0){
		esegui(buf, oF, eF);
	} else {
		pipeHandler(buf, index, oF, eF);
	}

return 1;
}


int main(int argc, char *argv[]){

	struct command *buf = malloc(128 * sizeof(char));
	int status=0;
	int *index=0;
	char *m_cwd = malloc(128 * sizeof(char));
	
	char *outPath=malloc(128*sizeof(char));
	char *errPath=malloc(128*sizeof(char));

	int oF;
	int eF;
	
	argCheck(argc, argv, outPath, errPath);
	fileOpen(outPath, errPath, &oF, &eF);

	system("clear");

	while(1){

		// INTESTAZIONE
		getcwd(m_cwd,128);
		printf("\033[1;36m");
		printf("LAURA_SHELL @ ");
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
