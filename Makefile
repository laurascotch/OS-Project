.PHONY: build clean help 

BOLD_RED=\033[1;31m
BOLD_GREEN=\033[1;32m
GREEN=\033[0;32m
PINK=\033[0;35m
BOLD_PINK=\033[1;35m
CYAN=\033[0;36m
BOLD_CYAN=\033[1;36m
NC=\033[0m

build: laura_shell help
	@if [ -d "bin" ]; then \
		rm -r bin; \
	fi
	@mkdir bin;
	@mv laura_shell bin/laura_shell
	@make clean

laura_shell.o: src/laura_shell.c
	@gcc -c src/laura_shell.c -w

catch.o: src/catch.c
	@gcc -c src/catch.c -w

comandi.o: src/comandi.c
	@gcc -c src/comandi.c -w

laura_shell: laura_shell.o comandi.o catch.o 
	@gcc laura_shell.o catch.o comandi.o -o laura_shell -w

clean: laura_shell.o catch.o comandi.o
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
	@echo "\t$(BOLD_PINK)./laura_shell -o=\"file.txt\" -e=\"e.txt\"$(NC)"
	@echo "Verranno creati due file di log:"
	@echo "-  $(GREEN)f.txt$(NC)\tContiene uno storico dei comandi eseguiti con successo"
	@echo "-  $(GREEN)e.txt$(NC)\tContiene uno storico dei comandi la cui esecuzione ha restituito un errore"
	@echo "\nQuesta shell supporta correttamente il piping."
	@echo "$(BOLD_CYAN)\nComandi utili:$(NC)"
	@echo "$(CYAN)exit$(NC)\tEsce dalla shell"
	@echo "$(CYAN)help$(NC)\tFornisce informazioni sulla shell"
