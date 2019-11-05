#include "bbuff.h"
#include <stdio.h>
#include <semaphore.h>

void* buff[BUFFER_SIZE];
sem_t mutex;
sem_t full;
sem_t empty;
int next_producer;
int next_consumer;
int item_num_in_buffer;

void bbuff_init(void){
    sem_init(&mutex,0,1);
    sem_init(&full,0,BUFFER_SIZE);
    sem_init(&empty,0,0);
    next_consumer = 0;
    next_producer = 0;
    item_num_in_buffer = 0;
}

void bbuff_blocking_insert(void* item){
    sem_wait(&full);
    sem_wait(&mutex);

    //critical
    if(next_producer == BUFFER_SIZE){
    	next_producer = 0;
    	buff[next_producer] = item;
    	next_producer++;
    	item_num_in_buffer++;
    }
    else{
    	buff[next_producer] = item;
    	next_producer++;
    	item_num_in_buffer++;
    }

    sem_post(&mutex);
    sem_post(&empty);
}

void* bbuff_blocking_extract(void){
    void* extract_item;
    sem_wait(&empty);
    sem_wait(&mutex);

    //critical
    if(next_consumer == BUFFER_SIZE){
    	next_consumer = 0;
    	extract_item = buff[next_consumer];
    	next_consumer++;
    	item_num_in_buffer--;
    }
    else{
    	extract_item = buff[next_consumer];
    	next_consumer++;
    	item_num_in_buffer--;
    }

    sem_post(&mutex);
    sem_post(&full);
    return extract_item;
}

_Bool bbuff_is_empty(void){
    return (item_num_in_buffer==0);
}



