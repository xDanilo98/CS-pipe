  
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



#ifndef _worker_c
#define _worker_c

void* worker (void*arg) {

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
		printf("SERVER %d CLOSING %lui ESTIMATE %d\n", (i+1), id, min);
		msg m = {id,min};
		CHECKLOCK1(pthread_mutex_lock(&lock));
		write(apipe[i][1], &m, sizeof(msg));
		CHECKLOCK1(pthread_mutex_unlock(&lock));
	}

	pthread_exit(NULL);

	
}

#endif
