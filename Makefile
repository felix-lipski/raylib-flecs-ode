scf4: exe
	./exe

exe: src/main.c
	gcc -c -o src/main.o src/main.c -I.
	g++ -o exe src/main.o -lraylib -lode -lm -lpthread -lflecs

clean:
	rm exe src/main.o
