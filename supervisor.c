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
#include <arpa/inet.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"info.h"
#include<endian.h>
#include<stdint.h>
#include <time.h>
#include <sys/time.h>
#include "server.h"
#include<signal.h>


#ifndef _supervisor_c
#define _supervisor_c

int k,dimtab;

int* servers;

typedef struct _stima{
    uint64_t id;
    int stim;
    int numserv;
} stima;

stima* tabstim;

void printout () {
	int i;
	for (i=0;i<dimtab;i++) {
		fprintf(stdout, "SUPERVISOR ESTIMATE %d FOR %lui BASED ON %d\n", tabstim[i].stim,tabstim[i].id,tabstim[i].numserv);
	}
}

void printerr () {
	int i;
	for (i=0;i<dimtab;i++) {
		fprintf(stdout, "SUPERVISOR ESTIMATE %d FOR %lui BASED ON %d\n", tabstim[i].stim,tabstim[i].id,tabstim[i].numserv);
	}
}

volatile sig_atomic_t sig_c;

void sigalarm_handler(int signum){
	sig_c=1;
}

void sigint_handler (int signum) {
	if (sig_c<2) {
		sig_c = sig_c+1;
		alarm(1);
		
		printerr();
	}
	else if(sig_c==2){		
		printf("SUPERVISOR EXITING\n");
		int i;
		
		printout();

		for(i=0;i<k;i++)
			kill(servers[i],9);
		
		_exit(EXIT_SUCCESS);
	}
}

void insert(msg m){
	
	int i = 0;
	int stima = m.estim;
	uint64_t id = m.id;
	int found = 0;
	
	while (i<dimtab && !found) {
		if (id == tabstim[i].id) {
			if(stima<tabstim[i].stim){
				tabstim[i].stim=stima;
			}
			tabstim[i].numserv++;
			found=1;
		}
		else i++;
	}

	if (!found) {
		
		tabstim[dimtab].id = id;
		tabstim[dimtab].stim = stima;
		tabstim[dimtab].numserv++;
		
		dimtab++;
		
	}
}

int main (int argc, char *argv[]) {


	sig_c=1;

	struct sigaction sint;
	bzero(&sint, sizeof(sint));
	sint.sa_handler = sigint_handler;
	SIG_ACTION(SIGINT,sint);

	struct sigaction salarm;
	bzero(&salarm,sizeof(salarm));
	salarm.sa_handler = sigalarm_handler;
	SIG_ACTION(SIGALRM,salarm);

	struct sigaction sigpipe;
	bzero(&sigpipe,sizeof(sigpipe));
	sigpipe.sa_handler=SIG_IGN;
	SIG_ACTION(SIGPIPE,sigpipe);

    int i;
	sscanf(argv[1],"%d",&k);
	printf("SUPERVISOR STARTING %d\n", k);
	apipe = (int **) malloc(k*sizeof(int*));

    for(i=0;i<k;i++) {
		apipe[i] = (int *) malloc(2*sizeof(int));
		pipe(apipe[i]);
	}

	servers = (int *) malloc(k*sizeof(int));

	for(i=0;i<k;i++) {
		server(i, servers);
	}

	for (i=0;i<k;i++) {
		int flags = fcntl(apipe[i][0], F_GETFL, 0);
		fcntl(apipe[i][0], F_SETFL, flags | O_NONBLOCK);
	}


	tabstim=malloc(1024*sizeof(stima));
	for(int p=0;p<1024;p++){
		tabstim[p].id=0;
		tabstim[p].stim=0;
		tabstim[p].numserv=0;
	}
	
	dimtab=0;

    msg m;

	int e;

    while (1) {
		for (i=0;i<k;i++) {
			if((e=(read(apipe[i][0],&m,sizeof(msg)))) == sizeof(msg)) {
				if(e==-1){
					fprintf(stdout,"error read\n");
				}
				printf("SUPERVISOR ESTIMATE %d FOR %lui FROM %d\n", m.estim,m.id,(i+1));
				insert(m);
			}
		}
	}
	
	return 0;
}

#endif
