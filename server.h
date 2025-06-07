#ifndef SERVER_H
#define SERVER_H
#include"shared.h"

void *encrypter_thread(void* arg);
void createPrintablePass(char *password, int len);
void print_bytes(const char* str,const char* data, int len);

#endif