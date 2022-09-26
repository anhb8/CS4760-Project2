CC = gcc
CFLAGS = -I -g
all: main
main: main.o 
	$(CC) $(CFLAGS) main.c -o master

clean:  
	rm -f master *.o *.dat a.out
