all: assembler

assembler: assembler.o preAssembler.o firstPass.o secondPass.o tables.o words.o
	gcc assembler.o preAssembler.o firstPass.o secondPass.o tables.o words.o -o assembler

assembler.o: assembler.c
	gcc -ansi -Wall -pedantic -c assembler.c

preAssembler.o: preAssembler.c
	gcc -ansi -Wall -pedantic -c preAssembler.c

firstPass.o: firstPass.c
	gcc -ansi -Wall -pedantic -c firstPass.c

secondPass.o: secondPass.c
	gcc -ansi -Wall -pedantic -c secondPass.c

tables.o: tables.c
	gcc -ansi -Wall -pedantic -c tables.c

words.o: words.c
	gcc -ansi -Wall -pedantic -c words.c

clean: 
	rm *.o assembler

cleanfiles:
	rm *.am *.ob *.ent *.ext
