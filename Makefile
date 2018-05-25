.PHONY: build clean help 

build: laura_shell.out help
	@mkdir bin;
	@mv laura_shell.out bin/laura_shell.out
	@make clean

laura_shell.o: src/laura_shell.c
	@gcc -c src/laura_shell.c -w

catch.o: src/catch.c
	@gcc -c src/catch.c -w

comandi.o: src/comandi.c
	@gcc -c src/comandi.c -w

laura_shell.out: laura_shell.o comandi.o catch.o 
	@gcc laura_shell.o catch.o comandi.o -o laura_shell.out -w

clean: laura_shell.o catch.o comandi.o
	@rm *.o

help: 
	@echo "Informazioni sul progetto:"
	@echo "\tCarrara Andrea"
	@echo "\tFerrai Dario"
	@echo "\tLongato Matteo"
	@echo "\tScoccianti Laura"
	@echo "\nPer eseguire la shell è necessario digitare a riga di comando:"
	@echo "\t./laura_shell.out -o=\"file.txt\" -e=\"e.txt\""
	@echo "Verranno creati due file di log:"
	@echo "-  f.txt\tContiene uno storico dei comandi eseguiti con successo"
	@echo "-  e.txt\tContiene uno storico dei comandi la cui esecuzione ha restituito un errore"
	@echo "\nQuesta shell supporta correttamente il piping."
	@echo "\nComandi utili:"
	@echo "exit\tEsce dalla shell"
	@echo "help\tFornisce informazioni sulla shell"
