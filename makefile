all:
	mkdir -p bin
	gcc -o bin/tetris src/*.c  -I/usr/include/SDL2 -D_REENTRANT -pthread -lSDL2main -lSDL2 -lSDL2_mixer  -lSDL2_image 
run:
	./bin/tetris
remove:
	$(RM) bin/tetris
