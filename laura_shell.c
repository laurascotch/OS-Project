#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "comandi.h"
#include "catch.h"


int launch(struct command *buf, int index) {
	
	if(index==0){
		esegui(buf);
	} else {
		pipeHandler(buf, index);
	}

return 1;
}


int main(void){

	struct command *buf = malloc(128 * sizeof(char));
	int status=0;
	int *index=0;
	char *m_cwd = malloc(128 * sizeof(char));

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
		
		//printf("index rit: %d\n", index);


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
			status=launch(buf, index);
		}
	}


return 0;
}
