CC=gcc
CLEAN=rm -f
PROGRAM_NAME=file_manager

$(PROGRAM_NAME): fm_func.o main.o
	$(CC) -o $(PROGRAM_NAME) -g fm_func.o main.o -lpanel -lncurses -lpthread
fm_func.o: fm_func.c fm_func.h
	$(CC) -c fm_func.c -lpanel -lncurses -lpthread
main.o: main.c
	$(CC) -c main.c -lpanel -lncurses -lpthread
clean:
	$(CLEAN) *.o
	$(CLEAN) $(PROGRAM_NAME)