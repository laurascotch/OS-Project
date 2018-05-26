.PHONY: build clean help 

BOLD_RED=\033[1;31m
BOLD_GREEN=\033[1;32m
GREEN=\033[0;32m
PINK=\033[0;35m
BOLD_PINK=\033[1;35m
CYAN=\033[0;36m
BOLD_CYAN=\033[1;36m
NC=\033[0m

build: shell help
	@if [ -d "bin" ]; then \
		rm -r bin; \
	fi
	@mkdir bin;
	@mv shell bin/shell
	@make clean

shell_main.o: src/shell_main.c
	@gcc -c src/shell_main.c -w

execute.o: src/execute.c
	@gcc -c src/execute.c -w

utilities.o: src/utilities.c
	@gcc -c src/utilities.c -w

shell: shell_main.o utilities.o execute.o 
	@gcc shell_main.o execute.o utilities.o -o shell -w

clean: shell_main.o execute.o utilities.o
	@rm *.o

help: 
	@echo "$(BOLD_RED)INFORMAZIONI SUL PROGETTO$(NC)"
	@echo "\t$(BOLD_GREEN)Realizzato da:$(NC)"
	@echo "\tCarrara Andrea"
	@echo "\tFerrai Dario"
	@echo "\tLongato Matteo"
	@echo "\tScoccianti Laura"
	@echo "\nL'eseguibile si trova nella cartella $(BOLD_GREEN)bin$(NC)"
	@echo "\nPer $(PINK)eseguire$(NC) la shell è necessario digitare a riga di comando:"
	@echo "\t$(BOLD_PINK)./shell -o=\"file.txt\" -e=\"error.txt\"$(NC)"
	@echo "Verranno creati due file di log:"
	@echo "-  $(GREEN)file.txt$(NC)\tContiene uno storico dei comandi eseguiti con successo"
	@echo "-  $(GREEN)error.txt$(NC)\tContiene uno storico dei comandi la cui esecuzione ha restituito un errore"
	@echo "\nQuesta shell supporta correttamente il piping."
	@echo "$(BOLD_CYAN)\nComandi utili:$(NC)"
	@echo "$(CYAN)exit$(NC)\tEsce dalla shell"
	@echo "$(CYAN)help$(NC)\tFornisce informazioni sulla shell\n"
