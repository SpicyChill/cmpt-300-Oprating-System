#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "bbuff.h"
#include "stats.h"

#define LIMIT_F 4;
#define LIMIT_K 2;


_Bool stop_thread_fac = false;

typedef struct {
    int factory_number;
    double time_stamp_in_ms;
} candy_t;

double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void Factory_thread(candy_t *candy){
	while(!stop_thread_fac){
		int wait_time_f = 0;
		wait_time_f = rand() % LIMIT_F;
		printf("Factory %d ships candy & waits %ds \n", candy->factory_number, wait_time_f);

		bbuff_blocking_insert(candy);
		candy->time_stamp_in_ms = current_time_in_ms();
		stats_record_produced(candy->factory_number);
		sleep(wait_time_f);
	}
	printf("Candy-factory %d done \n", candy->factory_number);
}

void Kid_thread(void){
	while(true){
		int wait_time_k = 0;
		wait_time_k = rand() % LIMIT_K;
		candy_t *out_candy;
		out_candy = bbuff_blocking_extract();
		double current_time = current_time_in_ms();
		//printf("current_time:%f\n", current_time);
		//printf("candy_time:%f\n", out_candy->time_stamp_in_ms);
		double delay = current_time - (out_candy->time_stamp_in_ms);
		//printf("delay:%f\n", delay);
		stats_record_consumed(out_candy->factory_number, delay);

		//printf("Num is %d, and Time is %lf \n", out_candy->factory_number, out_candy->time_stamp_in_ms);
		sleep(wait_time_k);
	}
}




int main(int argc,char*argv[]){

    if((atoi(argv[1])<1)||(atoi(argv[2])<1)||(atoi(argv[3])<1)){
        printf("Error: All arguments must be positive.\n");
        exit(EXIT_FAILURE);
    }
    else if(argc<4){
        printf("Require more arguments\n");
        exit(EXIT_FAILURE);
    }

    int factories_number = atoi(argv[1]);

    int kids_number = atoi(argv[2]);

    int seconds = atoi(argv[3]);

    pthread_t *factories;
    factories = malloc(factories_number * sizeof(*factories));
 	
    pthread_t *kids;
    kids = malloc(kids_number * sizeof(kids));

    //initialize
    bbuff_init();
    stats_init(factories_number);

    //raise factory
    int facthread_counter[factories_number];
    for(int i=0; i<factories_number; i++){
    	facthread_counter[i] = i;
    }

    for(int i=0; i<factories_number; i++){
    	candy_t *my_candy = malloc(sizeof(candy_t));
    	my_candy->factory_number = i;
    	my_candy->time_stamp_in_ms = current_time_in_ms();
    	//stats_record_produced(my_candy->factory_number);
    	int fac_return = pthread_create(&factories[facthread_counter[i]], NULL, (void *)Factory_thread, my_candy);
    	if(fac_return != 0){
    		printf("Fac thread create error");
    		exit(1);
    	}
    }

    //raise kid
    int kidthread_counter[kids_number];
    for(int i=0; i<kids_number; i++){
    	kidthread_counter[i] = i;
    }

    for(int i=0; i<kids_number; i++){
    	//printf("xixi\n");
    	int kid_return = pthread_create(&kids[kidthread_counter[i]], NULL, (void *)Kid_thread, NULL);
    	//printf("haha\n");
    	if(kid_return != 0){
    		printf("Kid thread create error");
    		exit(1);
    	}
    }

    //wait
    for(int i=0; i<seconds; i++){
    	printf("Time %ds\n", i);
    	sleep(1);
    	
    }

    //kill factory
    printf("Stop factories now\n");
	stop_thread_fac = true;
	for(int i=0; i<factories_number; i++){
		pthread_join(factories[facthread_counter[i]], NULL);
	}

	//check empty
	while(true){
		bool finish = bbuff_is_empty();
		if(finish == true){
			printf("Empty Buffer now\n");
			break;
		}
		else{
			printf("Waiting for all candy to be consumed\n");
			sleep(1);
		}
	}

	//kill kid
	printf("Stop kids now\n");
	for(int i=0; i<kids_number; i++){
		pthread_cancel(kids[kidthread_counter[i]]);
		pthread_join(kids[kidthread_counter[i]], NULL);
	}


	stats_display();
	stats_cleanup();
}
