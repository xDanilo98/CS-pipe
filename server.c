  
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



#ifndef _server_c
#define _server_c



static void* worker (void*arg) {

    struct timeval t;
    uint64_t buffid=0, id=0;
	int j=0, n=0;
	int* temp = (int*) arg;
	int fd = temp[0];
	int i = temp[1];
	int fstim=0,min=999999,tempstima=0;
	
	while ((n = read(fd, &buffid, sizeof(uint64_t))) == sizeof(uint64_t)) {
		gettimeofday(&t,NULL);
		if (j==0){
            id=be64toh(buffid);
			tempstima=((t.tv_sec)*1000+(t.tv_usec/1000));
        }
		if(j!=0){
			fstim=((t.tv_sec)*1000+(t.tv_usec/1000));
			int lol=fstim-tempstima;
			if(lol>0 && lol<min){
				min=lol;
				tempstima=fstim;
			}
		}
		j++;
	}
	
	close(fd);

	if (min>0) {
		printf("SERVER %d CLOSING %016llX ESTIMATE %d\n", (i+1), id, min);
		msg m = {id,min};
		write(apipe[i][1], &m, sizeof(msg));
	}

	pthread_exit(NULL);

	
}

void* dispatcher(void* args){

    int i;
    int fd;
	pthread_t t;
	int* temp = (int*) args;
	fd = temp[0];
	i = temp[1];
	
    while(1) {
		int cs;
		
		if ((cs = accept(fd,NULL,NULL)) == -1) {
			perror("dispatcher: accept error");
		}
		
		if (cs > 0) {
			printf("SERVER %d CONNECT FROM CLIENT\n", (i+1));
			int arr[2];
			arr[0] = cs;
			arr[1] = i;
			pthread_create(&t,NULL,&worker,arr);
		}
	}
	
	pthread_exit(NULL);

}

void server(int i,int servers[]){

    servers[i] = fork();

    if(servers[i]==0){

        fprintf(stdout,"inizializzo server n:%d\n",(i+1));

        pthread_t t;

        struct sockaddr_un sock;
        sock.sun_family=AF_UNIX;
        char name[16] = "OOB-server-";
        sprintf((name+11), "%d\0", (i+1));
        strncpy(sock.sun_path,name,108);

        int socket_sv=0;
        CHECKSOCKET(socket_sv=socket(AF_UNIX,SOCK_STREAM,0));
        CHECKBIND(bind(socket_sv,(struct sockaddr*)&sock,sizeof(sock)));
        CHECKLISTEN(listen(socket_sv,SOMAXCONN));

        int arr[2];
        arr[0] = socket_sv;
        arr[1] = i;         
        
        //occhio a SIGINT

        pthread_create(&t,NULL,&dispatcher,arr);
        pthread_join(t,NULL);
        exit(0);
    }

}

#endif
