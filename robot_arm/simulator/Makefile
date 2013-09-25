
simulate : main.o forward.o stacker.o header.h
	gcc main.o forward.o stacker.o header.h -o simulator -lm

main.o : main.c header.h
	gcc -c main.c -lm

forward.o : forward.c header.h
	gcc -c forward.c -lm

stacker.o : stacker.c header.h
	gcc -c stacker.c
