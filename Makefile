FLAGS := -g
OPTIMISE := -O0
LIBS := -lraylib -lgdi32 -lwinmm

build/main: src/main.c
	powershell -c New-Item -ItemType Directory -Force -Path .\build
	clear
	gcc -o build/main src/main.c $(LIBS) $(FLAGS) $(OPTIMISE) 

clear: build/main
	clear
	rm build/main