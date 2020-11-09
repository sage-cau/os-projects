#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int gNumber = -1;
int pSum = 0;
int cSum = 0;

void *producer(void *arg)
{
	srand((long)time(NULL));
	
    for(int i = 0; i < 100; i++){
        while (gNumber >= 0){
            usleep(1000);
		}
        gNumber = rand() % 100;
		printf("%d ", gNumber);
        pSum += gNumber;
    }
    pthread_exit(&pSum);
}

void *consumer(void *arg)
{
    for(int i = 0; i < 100; i++){
        while (gNumber < 0){
            usleep(1000);
		}
		printf("%d ", gNumber);
        cSum += gNumber;
		gNumber = -1;
    }
    pthread_exit(&cSum);
}


int main(int argc, char *argv[])
{
    pthread_t threads[2];
    int res;
    int *gSum1, *gSum2;

    res = pthread_create(&threads[0], NULL, producer, NULL);
    res = pthread_create(&threads[1], NULL, consumer, NULL);
	printf("waiting for thread to finish\n");

    res = pthread_join(threads[0], (void*)&gSum1);
    res = pthread_join(threads[1], (void*)&gSum2);

	printf("\n");
	printf("gSum1: %d\n", *gSum1);
	printf("gSum2: %d\n", *gSum2);
    
    if (*gSum1 == *gSum2)
        printf("success\n");
    else
        printf("fail\n");
}

