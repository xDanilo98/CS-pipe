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

dispatcher.o: dispatcher.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g dispatcher.c

worker.o: worker.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g worker.c

mainclient: client.o
	$(CC) -o mainclient client.o

client.o: client.c $(DEPPS)
	$(CC) $(CFLAGS) -c -g client.c

perm:
	@chmod +x test.sh

clean:
	@rm -f main OOB-* *.o mainclient *.log supser* form* clio*

test:
	make all
	./test.sh

