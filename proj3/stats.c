#include <stdio.h>
#include <stdlib.h>
#include "stats.h"

typedef struct {
int candy_num_made;
int candy_num_eaten;
double min_delay;
double ave_delay;
double max_delay;
double total_delay;
}stat_data;

stat_data* collection;
int count = 0;

void stats_init(int num_producers){
     collection= malloc(sizeof(stat_data)*num_producers);
     count = num_producers;
     for(int i=0;i<num_producers;i++){
        collection[i].candy_num_made = 0;
        collection[i].candy_num_eaten = 0;
        collection[i].min_delay = 99999999;
        collection[i].ave_delay = 0;
        collection[i].max_delay = 0;
        collection[i].total_delay = 0;
     }
}

void stats_cleanup(void){
    free(collection);
}

void stats_record_produced(int factory_number){
    collection[factory_number].candy_num_made++;
    //printf("jinlaila\n");

}

void stats_record_consumed(int factory_number, double delay_in_ms){
	//printf("chuqula\n");
    collection[factory_number].candy_num_eaten++;
    collection[factory_number].total_delay+=delay_in_ms;

    if(collection[factory_number].min_delay > delay_in_ms){
        collection[factory_number].min_delay = delay_in_ms;
        //printf("minidelay:%f\n", delay_in_ms);
    }

    else if(collection[factory_number].max_delay<delay_in_ms){
        collection[factory_number].max_delay = delay_in_ms;
        //printf("maxdelay:%f\n", delay_in_ms);
    }

    //printf("chuqula\n");
}

void stats_display(void){
    printf("Statistics:\n");
    printf("%8s%10s%10s%20s%20s%20s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
    for(int i=0;i<count;i++){
    	if(collection[i].candy_num_made != collection[i].candy_num_eaten){
    		printf("ERROR: Mismatch between number made and eaten.\n");
    		break;
    	}
    	else{
    		collection[i].ave_delay = collection[i].total_delay/collection[i].candy_num_eaten;
        	printf("%8d%10.5d%10.5d%20.5f%20.5f%20.5f\n", i, collection[i].candy_num_made, collection[i].candy_num_eaten, collection[i].min_delay, collection[i].ave_delay, collection[i].max_delay);
    	}
        
    }
}