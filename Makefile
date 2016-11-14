CC=gcc
CLEAN=rm -f
PROGRAM_NAME=file_manager

$(PROGRAM_NAME): fm_func.o main.o
	$(CC) -o $(PROGRAM_NAME) -g fm_func.o main.o -lpanel -lncurses
fm_func.o: fm_func.c fm_func.h
	$(CC) -c fm_func.c -lpanel -lncurses
main.o: main.c
	$(CC) -c main.c -lpanel -lncurses
clean:
	$(CLEAN) *.o
	$(CLEAN) $(PROGRAM_NAME)