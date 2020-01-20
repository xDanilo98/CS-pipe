  
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
#include "worker.h"



#ifndef _dispatcher_c
#define _dispatcher_c

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

#endif
