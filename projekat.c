// OPERATIVNI SISTEMI PROJEKAT
// ZELJKO MARKOVIC 2025220010

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <pthread.h>
#include <time.h>
#include "common.h"
#include "common_threads.h"

//globalne promjenljive
int max; //velicina bafera
int loops; //broj elemenata koje producent pravi
int *buffer; //bafer
int use_ptr = 0; //indeks za citanje
int fill_ptr = 0; //indeks za pisanje
int num_full = 0; //broj popunjenih mjesta u baferu

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv_full = PTHREAD_COND_INITIALIZER; //budi konzumenta kada se pojavi novi podatak u baferu
pthread_cond_t cv_empty = PTHREAD_COND_INITIALIZER; //budi producenta kada se otvori mjesto u baferu
//cv_full i cv_empty ne trose resurse stalnim provjeravanjem da li se ispunio odredjeni uslov

int priority_waiting = 0; //da li prioritetni ceka
int num_regular = 0; //broj regularnih konzumenata
int scenario = 1; //scenario 1 ili 2

//pomocne funkcije
void do_fill(int value){
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % max;
    num_full++;
}

int do_get(){
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % max;
    num_full--;
    return tmp;
}

//producent
void *producer(void *arg){
    int i;
    for(i = 0; i < loops; i++){
        int value = rand() % 100; //nasumicna vrijednost 0-99

        Mutex_lock(&m);
        while(num_full == max)
        Cond_wait(&cv_empty, &m);

        do_fill(value);
        printf("producent: proizveo %d\n", value);

        pthread_cond_broadcast(&cv_full); //ne koristimo Cond_signal() jer imamo vise niti kojima
        //producent javlja da je slobodno
        Mutex_unlock(&m);
    }

    Mutex_lock(&m);
    do_fill(-1);//-1 je signal za kraj
    printf("producent: zavrsio\n");
    pthread_cond_broadcast(&cv_full);
    Mutex_unlock(&m);

    return NULL;
}

//prioritetni konzument
void *priority_consumer(void *arg){
    int tmp = 0;

    while (tmp != -1){
        Mutex_lock(&m);

        priority_waiting++; //prioritetni ceka
        while (num_full == 0)
        Cond_wait(&cv_full, &m);
        priority_waiting--; //prioritetni vise ne ceka

        tmp = do_get();
        Cond_signal(&cv_empty);
        Mutex_unlock(&m);

        if(tmp != -1)
        printf("prioritetni: uzeo %d\n", tmp);
    }

    printf("prioritetni: zavrsio\n");
    return NULL;
}

//regularni konzument
void *regular_consumer(void *arg){
    int id = *(int *)arg;
    int tmp = 0;

    while (tmp != -1){
        Mutex_lock(&m);

        //regularni ceka ako je bafer prazan ili ako prioritetni ceka
        while(num_full == 0 || priority_waiting > 0)
        Cond_wait(&cv_full, &m);

        tmp = do_get();
        Cond_signal(&cv_empty);
        Mutex_unlock(&m);

        if(tmp != -1)
        printf("regularni %d: uzeo %d\n", id, tmp);
    }

    printf("regularni %d: zavrsio\n", id);
    return NULL;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "GRESKA: unos <scenario> <velicina bafera>\n");
        exit(1);
    }

    scenario = atoi(argv[1]);
    max = atoi(argv[2]);
    buffer = malloc(max * sizeof(int)); //alokacija memorije za bafer
    assert(buffer != NULL);//provjerava da li je malloc() uspijesan

    srand(time(NULL));//treba nam da ne bi stalno dobijali iste brojeve

    pthread_t prod;
    pthread_t prior;
    pthread_t regular[5];
    int ids[5];//niz za ID-eve regularnih konzumenata

    //scenario 1
    if(scenario == 1){
        loops = 10;// broj stavki koje producent pravi
        num_regular = 1;

        printf("1 producent, 1 regularni konzument, 1 prioritetni konzument\n");

        Pthread_create(&prod, NULL, producer, NULL);
        Pthread_create(&prior, NULL, priority_consumer, NULL);

        ids[0] = 1;
        Pthread_create(&regular[0], NULL, regular_consumer, &ids[0]);

        Pthread_join(prod, NULL);
        Pthread_join(prior, NULL);
        Pthread_join(regular[0], NULL);
    }

    //scenario 2
    else if(scenario == 2){
        loops = 50;
        num_regular = 5;

        printf("1 producent, 5 regularnih konzumenata, 1 prioritetni konzument\n");

        Pthread_create(&prod, NULL, producer,NULL);
        Pthread_create(&prior, NULL, priority_consumer, NULL);

        int i;
        for(i = 0; i < num_regular; i++){
            ids[i] = i + 1;
            Pthread_create(&regular[i], NULL, regular_consumer, &ids[i]);
        }

        Pthread_join(prod, NULL);
        Pthread_join(prior, NULL);
        for(i = 0; i < num_regular; i++)
        Pthread_join(regular[i], NULL);
    }

    else{
        printf("GRESKA: scenario moze biti 1 ili 2\n");
    }

    free(buffer);
    printf("zavrseno\n");
    return 0;
}