  
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "info.h"
#include <endian.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include "dispatcher.h"
#include <signal.h>




#ifndef _server_c
#define _server_c


/*Esegue le fork per creare il processo server distinto crea la socket e si mette in ascolto,
viene mascherato SIGINT in caso il processo dovesse terminare tramite un ctrl-c e crea un thread dispatcher
per accettare le connessioni */
void server(int i,int servers[]){

    servers[i] = fork();

    if(servers[i]==0){

        fprintf(stdout,"SERVER %d ACTIVE\n",(i+1));

        pthread_t t;

        struct sockaddr_un sock;
        sock.sun_family=AF_UNIX;
        char name[16] = "OOB-server-";
        sprintf((name+11), "%d", (i+1));
        strncpy(sock.sun_path,name,108);

        int socket_sv=0;
        CHECKSOCKET(socket_sv=socket(AF_UNIX,SOCK_STREAM,0));
        CHECKBIND(bind(socket_sv,(struct sockaddr*)&sock,sizeof(sock)));
        CHECKLISTEN(listen(socket_sv,SOMAXCONN));

        int arr[2];
        arr[0] = socket_sv;
        arr[1] = i;         
        
        sigset_t set;
        sigfillset(&set);
        pthread_sigmask(SIG_SETMASK,&set,NULL);

        pthread_create(&t,NULL,&dispatcher,arr);
        pthread_join(t,NULL);
    }

}

#endif
