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
#include <signal.h>
#include <fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"info.h"
#include<endian.h>
#include<stdint.h>
#include <time.h>
#include <sys/time.h>
#include<signal.h>
#include "server.h"


#ifndef _supervisor_c
#define _supervisor_c


int k;

volatile sig_atomic_t sig_counter;

volatile sig_atomic_t Sig;

//handler dei segnali
void sighandler(int sig){
    Sig=sig;
}

static void sigalrm_handler (int signum) {
	sig_counter=0;
}

static void sigint_handler (int signum) {
	if (!sig_counter) {
		sig_counter = 1;
		alarm(1);

        printf("pocco dio1\n");

	}
	else {
		write(1,"\n",1);
		
		printf("pocco dio2\n");
		
		write(1, "SUPERVISOR EXITING\n", 19);
		int i;
		
		for(i=0;i<k;i++)
			kill(servers[i],9);
		
		_exit(EXIT_SUCCESS);
	}
}

void sethandler(){
    struct sigaction sigterm;
    struct sigaction sigint;
    struct sigaction sigusr1;
    struct sigaction sigpipe;

    memset(&sigterm,0,sizeof(struct sigaction));
    memset(&sigint,0,sizeof(struct sigaction));
    memset(&sigusr1,0,sizeof(struct sigaction));
    memset(&sigpipe,0,sizeof(struct sigaction));

    sigpipe.sa_handler=SIG_IGN;
    sigterm.sa_handler=sighandler;
    sigint.sa_handler=sighandler;
    sigusr1.sa_handler=sighandler;


    SIG_ACTION(SIGPIPE,sigpipe);
    SIG_ACTION(SIGINT,sigint);
    SIG_ACTION(SIGUSR1,sigusr1);
    SIG_ACTION(SIGTERM,sigterm);
}

int* servers;

int main (int argc, char *argv[]) {

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

    //segnali

    sig_counter=0;


    msg m;

    while (1) {
		for (i=0;i<k;i++) {
			if(read(apipe[i][0],&m,sizeof(msg)) == sizeof(msg)) {
				printf("SUPERVISOR ESTIMATE %d FOR %016llX FROM %d\n", m.estim,m.id,(i+1));
				//insert(m);
			}
		}
	}
}

#endif