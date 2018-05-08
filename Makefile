COMPF = -Wall -Werror -Wextra

dir:
	@mkdir bin
	@cd bin

build:	shell.o
	@gcc -o shell shell.o $(COMPF)

shell.o: dir
	@gcc -c shell.c shell.h $(COMPF)

clean:
	@filename="shell"
	@directory=bin/tmp
	if [-d "$directory"]; then
		@rmdir $directory
	fi
	if [-f "$filename"]; then
		@rm $filename
	fi
	
	

