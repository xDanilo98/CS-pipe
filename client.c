#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <arpa/inet.h>

#define UNIX_PATH_MAX 108


/*
Funzione che controlla che i server scelti per la
spedizione dei messaggi siano tutti distinti
*/

int distinti (int i, int* arr) {
	int j = 0;
	int dist = 1;
	
	while (j<i && dist) {
		if (arr[j] == arr[i])
			dist = 0;
		j++;
	}
	
	return dist;
}


/*
Funzione che, se necessario, porta l'id (a 64 bit, per il quale non esiste
una funzione standard POSIX) da host byte order a network byte order. Per la
realizzazione vengono utilizzati due elementi di appoggio a 32 bit, contenenti
le due metà dell'id, sulle quali viene chiamata la funzione htonl.
Successivamente le due parti vengono scambiate tra loro e riunite.
*/

uint64_t htonll(uint64_t n_id) {
	
	int num = 42;
	
	if (*(char *)&num == 42) {
		uint32_t high_part = htonl((uint32_t)(n_id >> 32));
		uint32_t low_part = htonl((uint32_t)(n_id & 0xFFFFFFFFLL));
		return (((uint64_t)low_part) << 32) | high_part;
	}
	
	else {
        	return n_id;
	}
}


/*
Funziona che genera un id a 64 bit unico per il client, dal
momento che come seed usa il pid del processo stesso.
*/

uint64_t genera_id() {
	
	int pid = getpid();
	unsigned short seed[3] = {pid,pid,pid};
	
	seed48(seed);
	
	uint64_t hi = lrand48();
	uint64_t md = lrand48();
	uint64_t lo = lrand48();
	
	return ((hi << 42) + (md << 21) + lo);
}


/*
Procedura che genera un array di p indirizzi sa della famiglia AF_UNIX,
con sun_path=OOB-SERVER-i, a cui poi connette l'array di socket cs che
fornisce al main un modo di comunicare con uno dei p server scelti senza
preoccuparsi di aprire nuove connessioni per ogni messaggio.
*/

void create_addr(int k, int p, int* cs) {
	
	int i,j;
	struct sockaddr_un* sa = malloc (p*sizeof(struct sockaddr_un));
	int* chosen = malloc (p*sizeof(int));
	char s[15] = "OOB-server-";
	
	for (i=0; i<p; i++) {
		chosen[i] = (rand() % k) + 1;
		
		while (!distinti(i, chosen))
			chosen[i] = (rand() % k) + 1;
		
		sprintf((s+11), "%d\0", chosen[i]);
		strncpy(sa[i].sun_path,s,UNIX_PATH_MAX);
		sa[i].sun_family=AF_UNIX;
		
		if ((cs[i] = socket(AF_UNIX,SOCK_STREAM,0)) == -1)
			perror("client: error creating socket");
		
		if (connect(cs[i],(struct sockaddr*)&(sa[i]),sizeof(sa[i]))==-1)
			perror("client: error connecting to socket");
	}
}


//CLIENT

int main (int argc, char* argv[]) {
	
	uint64_t id, n_id;
	int secret, i, j, n, p, k, w;
	
	sscanf(argv[1],"%d",&p);
	sscanf(argv[2],"%d",&k);
	sscanf(argv[3],"%d",&w);
	
	srand(getpid());
	secret = (rand() % 3000) + 1;
	id = genera_id();
	
	printf("CLIENT %lui SECRET %d\n", id, secret);
	
	int* cs = malloc(p*sizeof(int));
	create_addr(k,p,cs);
	n_id = htonll(id);
	int usecret = secret*1000;
	
	//comunicazione con i server scelti
	for (i=0;i<w;i++) {
		j = rand()%p;
		if ((n = write(cs[j],&n_id,sizeof(uint64_t))) != sizeof(uint64_t))
			perror("client: error writing in the socket");
		usleep(usecret);
	}
	
	//chiusura delle socket
	for (i=0; i<p; i++)
		if (close(cs[i])==-1)
			perror("client:closing sockets");
	
	printf("CLIENT %lui DONE\n", id);
	
	return 0;
}
