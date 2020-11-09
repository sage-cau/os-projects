#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

sem_t bin_sem;
int balance = 100000000;

void enter_region() {
    asm(
        ".data\n"
        "lock:\n"
        ".byte 0\n"
        ".text\n"
        "_enter_region:\n"
        "movb $1, %al\n" // move 1 to AL
        "xchgb lock,%al\n"
        "cmp $0, %al\n"
        "jne _enter_region\n"
    );
    //sem_wait(&bin_sem);
}
 
void leave_region() {
    asm("movb $0, lock");
    //sem_post(&bin_sem);
}

int borrow_money(char *p) {
    int amount = 0;

    enter_region();
    printf("%s start its critical section (borrow)\n", p);
    amount = rand()%100 + 1;
    balance -= amount;
    printf("%s가 우리은행에서 %d만원을 빌렸습니다.\n", p, amount);
    usleep(rand()%1000000);
    printf("%s end its critical section (borrow)\n", p);
    leave_region();
    
    return amount;
}

void use_money(char *p, int amount) {
    printf("%s가 %d만원을 사용합니다.\n", p, amount);
    usleep(rand()%1000000);
}

int repay_money(char *p, int loan) {
    int amount = 0;

    enter_region();
    printf("%s start its critical section (repay)\n", p);
    do {
        amount = rand()%100 + 1;
    }while(amount > loan);
    balance += amount;
    printf("%s가 우리은행에 %d만원을 갚았습니다.\n", p, amount);
    usleep(rand()%1000000);
    printf("%s end its critical section (repay)\n", p);
    leave_region();

    return amount;
}
 
static void* f1(void* p) {
    int amount, loan = 0;

    for(int i = 0; i < 10; i++){
        switch(rand()%5) {
            case 0:
                if(loan > 0)
                    loan -= repay_money(p, loan);
                break;
            default:
                amount = borrow_money(p);
                loan += amount;
                use_money(p, amount);
                break;
        }
    }
    while(loan > 0)
        loan -= repay_money(p, loan);
    if(loan == 0)
        printf("====================================%s는 모든 돈을 갚았습니다.\n", (char*)p);

    return NULL;
}

static void* f2(void* p) {
    int amount, loan = 0;

    for(int i = 0; i < 10; i++){
        switch(rand()%5) {
            case 0:
                if(loan > 0)
                    loan -= repay_money(p, loan);
                break;
            default:
                amount = borrow_money(p);
                loan += amount;
                use_money(p, amount);
                break;
        }
    }
    while(loan > 0)
        loan -= repay_money(p, loan);
    if(loan == 0)
        printf("====================================%s는 모든 돈을 갚았습니다.\n", (char*)p);

    return NULL;
}
 
int main() {
    int init_balance = balance;
    int rc;
    pthread_t t1, t2, t3, t4, t5;
    srand((long)time(NULL));    //난수 생성 seed 설정

    sem_init(&bin_sem, 0, 1);
 
    rc = pthread_create(&t1, NULL, f1, "f1");
    if(rc != 0) {
        fprintf(stderr, "pthread f1 failed\n");
        return EXIT_FAILURE;
    }
 
    rc = pthread_create(&t2, NULL, f2, "f2");
    if(rc != 0) {
        fprintf(stderr, "pthread f2 failed\n");
        return EXIT_FAILURE;
    }

    rc = pthread_create(&t3, NULL, f1, "f3");
    if(rc != 0) {
        fprintf(stderr, "pthread f3 failed\n");
        return EXIT_FAILURE;
    }
    rc = pthread_create(&t4, NULL, f2, "f4");
    if(rc != 0) {
        fprintf(stderr, "pthread f4 failed\n");
        return EXIT_FAILURE;
    }
    rc = pthread_create(&t5, NULL, f2, "f5");
    if(rc != 0) {
        fprintf(stderr, "pthread f5 failed\n");
        return EXIT_FAILURE;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    puts("All threads finished.");
    sem_destroy(&bin_sem);

    printf("우리은행 초기잔고: %d만원 최종잔고: %d만원\n", init_balance, balance);
    if (init_balance == balance)
        printf("success\n");
    else
        printf("fail\n");
    return 0;
} 