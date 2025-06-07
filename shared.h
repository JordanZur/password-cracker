#ifndef SHARED_H
#define SHARED_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <mta_crypt.h>
#include <mta_rand.h>
#include <ctype.h>
#include <linux/time.h>
#include "server.h"


typedef struct{
char *encrypted_data;
unsigned int data_len;
int newDataReady;
int shouldExit;

pthread_mutex_t lock;
pthread_cond_t cond;
}SharedData;
typedef struct{
    SharedData *shared;
    int passLen;
    int timeOutSec;
}ThreadArgs;
typedef struct 
{
    
}Timespec;




#endif
