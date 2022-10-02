CC = gcc
CFLAGS = -I -g
OBJECTS= main.o master.o slave.o
SRC= config.h

all: master slave

%.o: %.c $(SRC)
	$(CC) $(CFLAGS) -c $< -o $@

master: master.o $(SRC)
	$(CC) $(CFLAGS) master.o -o master
	
slave: slave.o $(SRC)
	$(CC) $(CFLAGS) slave.o -o slave

clean:  
	rm -f master slave *.o  a.out cstest logfile.* 
