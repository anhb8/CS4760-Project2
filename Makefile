CC = gcc
CFLAGS = -I -g
all: master slave
master: master.o 
	$(CC) $(CFLAGS) master.c -o master
slave: slave.o
	$(CC) $(CFLAGS) slave.c -o slave

clean:  
	rm -f master slave *.o  a.out cstest logfile.*
