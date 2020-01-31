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
#include "info.h"

#ifndef _clientfin_c
#define _clientfin_c

#define UNIX_PATH_MAX 108

//procedura per attendere secret millisecondi
void wait_(int rand){
    struct timespec time;
    time.tv_sec = rand/1000;
    time.tv_nsec = (rand % 1000) * 1000000;
    nanosleep(&time,NULL);
}

//funzione per generare un id da 64 bit in modo pseudo-casuale
uint64_t generate_id(){
    uint64_t idi=mrand48();
    uint64_t idf=mrand48();
    uint64_t fin=(idi<<32)+idf;
    return fin;
}

int main (int argc, char* argv[]) {
	
	int secret;
    int i=0;
    int p=0;
    int k=0;
    int w=0;
	
	sscanf(argv[1],"%d",&p);
	sscanf(argv[2],"%d",&k);
	sscanf(argv[3],"%d",&w);

    if (p >= k || p <= 1 || w > 3*k){
        errno = EINVAL;
        perror("invalid args"); 
        exit(EXIT_FAILURE);
    }
	
	srand48(getpid());
    srand(getpid());
	secret = (rand() % 3000) + 1;
    uint64_t id=generate_id();
    uint64_t ID=htobe64(id);
		
	printf("CLIENT %lx SECRET %d\n", id, secret);
	
    int* server=NULL;
    CHECKMALLOC(server=malloc(p*sizeof(int)));
    int* Connect=NULL;
    CHECKMALLOC(Connect=malloc(k*sizeof(int)));

    //ciclo di connessione ai server
    while(i<p){
        int r=(lrand48()%k);
        if(Connect[r]==0){
            struct sockaddr_un sock;
            sock.sun_family=AF_UNIX;
            char name[16] = "OOB-server-";
            sprintf((name+11), "%d", (r+1));
            strncpy(sock.sun_path,name,108);

            int socket_sv=0;
            CHECKSOCKET(socket_sv=socket(AF_UNIX,SOCK_STREAM,0));

            if(connect(socket_sv, (struct sockaddr *)&sock, sizeof(sock)) < 0){
                perror("Connect");
                exit(EXIT_FAILURE);
            }

            Connect[r]=1;
            server[i]=socket_sv;
            i++;
        }
    }

    i=0;

    //ciclo per l'invio dei "w" messaggi
    while(i<w){
        int r=(lrand48()%p);
        if(server[r]!=-1){
            int a=write(server[r],&ID,8);
            if(a<=0){
                if(a==-1) perror("WRITE CLIENT");
                close(server[r]);
                server[r]=-1;
            }
            wait_(secret);
            i++;
        }
    }
	
	printf("CLIENT %lx DONE\n", id);

    i=0;
    for(i=0;i<p;i++){
        close(server[i]);
    }
    free(server);
    free(Connect);

	return 0;
}

#endif