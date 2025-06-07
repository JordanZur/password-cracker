
#include "server.h"


void *encrypter_thread(void *arg){


    ThreadArgs *args=(ThreadArgs*) arg;
    SharedData *shared =args->shared;
    int passLen=args->passLen;
    int keyLen= passLen/8;
    char *pass=malloc(passLen+1);
    char *key=malloc(keyLen); // its binary data not a string so not need for +1 fkme
    char *encrypetData=malloc(passLen*2 );
    unsigned int encryLen=0; // for now we dont know its acullty size right now 

    MTA_CRYPT_RET_STATUS res= MTA_crypt_init();
    assert(res==MTA_CRYPT_RET_OK);
    

    if(!pass || !key || !encrypetData){
        fprintf(stderr,"memorny alooction fail\n");
        return NULL;
    }
    while (1)
    {
        pthread_mutex_lock(&shared->lock);

        if(shared->shouldExit){
            pthread_mutex_unlock(&shared->lock);
            break;
        }
        struct timespec ts;
        prepare_timespec(&ts, args->timeOutSec);
        while (shared->newDataReady)
        {
            int rc=pthread_cond_timedwait(&shared->cond,&shared->lock,&ts);
            if(rc == ETIMEDOUT)
            {
                printf("Timeout reach. Regentaing password... \n");
                break;
            }

        }
        

        createPrintablePass(pass,passLen);//cooking a pass
        MTA_get_rand_data(key,keyLen);// coocking data

        res=MTA_encrypt(key,keyLen,pass,passLen,encrypetData, &encryLen);
        assert(res==MTA_CRYPT_RET_OK);
        memcpy(shared->encrypted_data,encrypetData, encryLen);
        shared->data_len=encryLen;//encryLen was update in MTA_encrypt
        shared->newDataReady=1;

        //notify the decrpyres 
        pthread_cond_broadcast(&shared->cond);
        pthread_mutex_unlock(&shared->lock);

        printf("%ld\t[SERVER] [INFO] New password generated: %s,",time(NULL),pass );
        print_bytes("key: ", key , keyLen);
        print_bytes("After encryption :", encrypetData, encryLen);
        printf("\n");

        sleep(3);//might change later
    }
    free(pass);
    free(key);
    free(encrypetData);
    
return NULL;
    
    
}
void createPrintablePass(char *password, int len){

    int i=0;
    while(i<len)
    {

        char curr= MTA_get_rand_char();

        if(curr >=33 && curr<= 126 )
        {
            password[i]=curr;
            i++;
        }
    }
    password[len]='\0';
    
}

void print_bytes(const char* str,const char* data, int len ){

    printf("%s",str);
    for(int i=0; i<len ; i++)
    {
        putchar(isprint(data[i]) ? data[i] : '.' );
    }
}


