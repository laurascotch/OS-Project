COMPF = -Wall -Werror -Wextra

dir:
	@mkdir bin

build:	shell
	@gcc -o bin/shell src/main.o src/comandi.o $(COMPF)

shell: dir
	@gcc -c src/main.c src/comandi.c $(COMPF)

clean:
	@rm bin/*
	

