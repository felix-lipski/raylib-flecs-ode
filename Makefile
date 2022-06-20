scf4: exe
	./exe

exe: src/main.c ecs/flecs.o
	gcc -o exe ecs/flecs.o src/main.c -I . -lraylib

ecs/flecs.o: ecs/flecs.c
	gcc -o ecs/flecs.o -c ecs/flecs.c

clean:
	rm exe ecs/flecs.o
