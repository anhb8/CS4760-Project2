CC = gcc
CFLAGS = -I -g
all: master
main: master.o 
	$(CC) $(CFLAGS) master.c -o master

clean:  
	rm -f master *.o *.dat a.out
