#include "shared.h"

SharedData shared;

int main(int argc, char* argv[]) {
    int numOfDecrypters = 0;
    int passLen = 0;
    int timeOut = 10;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--num-of-decrypters") == 0) && i + 1 < argc) {
            numOfDecrypters = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--password-length") == 0) && i + 1 < argc) {
            passLen = atoi(argv[++i]);
        } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--timeout") == 0) && i + 1 < argc) {
            timeOut = atoi(argv[++i]);
        } else {
            printf("Invalid or incomplete argument: %s\n", argv[i]);
            return 1;
        }
    }

    if (numOfDecrypters <= 0 || passLen <= 0) {
        printf("Usage: %s -n <numOfDecrypters> -l <passwordLength> [-t <timeout>]\n", argv[0]);
        return 1;
    }

    memset(&shared, 0, sizeof(shared));
    pthread_mutex_init(&shared.lock, NULL);
    pthread_cond_init(&shared.cond, NULL);

    // Allocate encrypted_data buffer
    shared.encrypted_data = malloc(passLen * 2);
    if (!shared.encrypted_data) {
        fprintf(stderr, "Failed to allocate encrypted_data buffer.\n");
        return 1;
    }

    // Init encryption library
    if (MTA_crypt_init() != MTA_CRYPT_RET_OK) {
        printf("Failed to initialize crypto.\n");
        return 1;
    }

    // Set up thread args
    ThreadArgs *args = malloc(sizeof(ThreadArgs));
    args->shared = &shared;
    args->passLen = passLen;
    args->timeOutSec = timeOut;

    pthread_t enc_thread;
    pthread_create(&enc_thread, NULL, encrypter_thread, args);

    // Run server for 20 seconds
    sleep(20);

    pthread_mutex_lock(&shared.lock);
    shared.shouldExit = 1;
    pthread_cond_broadcast(&shared.cond);
    pthread_mutex_unlock(&shared.lock);

    pthread_join(enc_thread, NULL);

    pthread_mutex_destroy(&shared.lock);
    pthread_cond_destroy(&shared.cond);
    free(shared.encrypted_data);
    free(args);

    return 0;
}