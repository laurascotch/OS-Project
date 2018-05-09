COMPF = -Wall -Werror -Wextra

dir:
	@mkdir bin

build:	shell.o
	@gcc -o bin/shell src/shell.o $(COMPF)

shell.o: dir
	@gcc -c src/shell.c src/shell.h $(COMPF)

clean:
	@rm bin/*
	

