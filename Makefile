CC = gcc
CFLAGS = -Wall
DEPPS = info.h

.PHONY: all

all: clean main mainclient

main: supervisor.o server.o dispatcher.o worker.o
	$(CC) -pthread -o main supervisor.o server.o dispatcher.o worker.o

supervisor.o: supervisor.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g supervisor.c

server.o: server.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g server.c

dispatcher.o: dispatcher.o $(DEPPS)
	$(CC) $(CFLAGS) -c -g dispatcher.c

worker.o: worker.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g worker.c

mainclient: client1.o
	$(CC) -o mainclient client1.o

client1.o: client1.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g client1.c
clean:
	@rm -f main OOB-* *.o mainclient
perm:
	@chmod +x test.sh
test:
	make all
	./main &
	./test.sh

