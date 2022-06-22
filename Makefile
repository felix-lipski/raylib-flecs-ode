scf4: exe
	./exe

exe: src/main.c ecs/flecs.o
	gcc -c -o src/main.o src/main.c -I.
	g++ -o exe ecs/flecs.o src/main.o -lraylib -lode -lm -lpthread

ecs/flecs.o: ecs/flecs.c
	gcc -o ecs/flecs.o -c ecs/flecs.c

clean:
	rm exe ecs/flecs.o src/main.o
