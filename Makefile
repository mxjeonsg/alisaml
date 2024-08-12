FLAGS := -g
OPTIMISE := -O0
INCLUDE := -Iinclude-external/raysan5-raylib
LIBS := -lraylib -lwinmm -lgdi32

build/main: src/main.c
	if [ -z "build" ]; then mkdir -p build; fi
	clear
	gcc -o build/main src/main.c $(LIBS) $(FLAGS) $(OPTIMISE) 

clear: build/main
	clear
	rm build/main