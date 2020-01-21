#ifndef _info_h_
#define _info_h_

#define LENNAME 108
#define DIMHASHTABLE 1024
#define N_LOCK 128


typedef struct Msg{
    uint64_t id;
	int estim;
} msg;


int** apipe;

pthread_mutex_t lock;

#define CHECKMALLOC(call) if((call)==NULL){ fprintf(stderr,"Error Malloc\n"); exit(EXIT_FAILURE);}

#define CHECKREAD(call) if((call)==-1) {fprintf(stderr,"Error in read\n");}

#define CHECKWRITE(X) if((X)==-1){fprintf(stderr,"Error in write\n");}

#define CHECKLOCKINIT(X) if((errno=(X))){fprintf(stderr,"Error lock init\n"); exit(EXIT_FAILURE);}

#define CHECKTHREAD(X,FD) if((X)!=0) {fprintf(stderr,"Error in create thread\n"); close(FD);}

#define CHECKSOCKET(call) if((call)==-1) {fprintf(stdout,"Error create Socket\n"); exit(EXIT_FAILURE);}

#define CHECKCONNECT(X) if((X)==-1) {fprintf(stdout,"Error in socket connect\n"); exit(EXIT_FAILURE);}

#define CHECKBIND(X) if((X)==-1) {fprintf(stderr,"Error in bind\n"); exit(EXIT_FAILURE);}

#define CHECKLISTEN(X) if((X)==-1) {fprintf(stderr,"Error in socketlisten\n"); exit(EXIT_FAILURE);}

#define CHECKLOCK(call,ret) if((call)!=0) { fprintf(stderr,"lock failed\n"); return ret; }

#define CHECKLOCK1(call) if((call)!=0) { fprintf(stderr,"lock failed\n");}

#define CHECKUNLOCK(call) if((call)!=0) fprintf(stderr,"unlock failed\n");

#define CHECKFCLOSE(X) if((X)!=0) { fprintf(stderr,"Error in close file\n");}

#define SIG_ACTION(SIG,X) if(sigaction(SIG,&X,NULL)==-1){perror("sigaction");}

#endif