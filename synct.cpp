//This is the code for Lab 4 in CS3080
//Author: Austin Carico
//November 1st, 2021
//This uses 6 producer threads and 6 consumer threads to produce
//and then consume random numbers and calculate their average, min and max

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

       
#define NUM_PRODUCERS 6
#define NUM_CONSUMERS 6
#define BSIZE 10                // Buffer size is 10
            

int numItems;
int range;
int bcounter = 0;
int buf[BSIZE];
int NextIn=0, NextOut=0;
int totMin = 1000;
int totMax = 0;
int totAvg = 0;
int total = 0;

void * producer(void *);        // function for producer thread
void * consumer(void *);        // function for consumer thread

pthread_t ptid[NUM_PRODUCERS]; // Producer ID's
pthread_t ctid[NUM_CONSUMERS];	//Consumer ID's

pthread_cond_t empty; 
pthread_cond_t full;
pthread_mutex_t lock;
pthread_t myID = pthread_self();

int main( int argc, char *argv[] ){ //start of main
  srand(myID);
  
  //initializing of conditions
  pthread_cond_init(&empty, NULL);
  pthread_cond_init(&full, NULL);
  pthread_mutex_init(&lock, NULL);
  //arguments taken in
  numItems = atoi(argv[1]);
  range = atoi(argv[2]);
  
  pthread_cond_signal(&empty);
  int i;
  printf("\n **** Main Program creating threads ++++ \n");
  //thread creation
  for(i = 0; i < NUM_PRODUCERS; i++){
	pthread_create(&ptid[i], NULL, producer, NULL);
  }
  for(i = 0; i < NUM_CONSUMERS; i++){
	pthread_create(&ctid[i], NULL, consumer, NULL);
  }
      
 //thread termination with pthread_join
  for(i = 0; i < NUM_PRODUCERS; i++){    //terminating threads
        pthread_join(ptid[i], NULL);
  }
  for(i = 0; i < NUM_CONSUMERS; i++){    //terminating threads
        pthread_join(ctid[i], NULL);
  }


  totAvg = total / (numItems * NUM_CONSUMERS);// calculate average
  printf("Overall values:\n Min of total: %d. Max of total: %d. Total Average: %d.\n", totMin, totMax,totAvg);
  pthread_exit(0);
  return 0;
} // end main

void * producer(void * parm)
{
  int num;
  printf(" \n+++Producer Started+++ \n");

  //produce item
  for(int j = 0; j < numItems; j++){
	pthread_mutex_lock(&lock);
	//check if buffer is full
	while(bcounter == BSIZE){ 
		printf("\n Producer found the buffer is full, is waiting currently\n");
		pthread_cond_wait(&empty, &lock);
        }
	bcounter++;
  	num = rand() % range;
  	buf[NextIn] = num;
  	NextIn = (NextIn + 1) %  BSIZE;
  	printf("  Produced item %d\n", num);
	pthread_cond_signal(&full);
  	pthread_mutex_unlock(&lock);
  	
  }

  printf("\n---Producer Exiting---\n");

  pthread_exit(0); 
}
void * consumer(void * parm)
{
  int count = 0;
  int cnum;
  
  printf("\n ---Consumer Started --- \n");
  //declaring local min max and avg variables
  int myMin = 1000;
  int myMax = 0;
  int myAvg;
  int myTotal;
  while(count < numItems){

	//consume item
 	cnum = buf[NextOut];
        if(cnum < myMin){
               myMin = cnum;
        }
        else if(cnum > myMax){
               myMax = cnum;
        }
        myTotal += cnum;

	
	pthread_mutex_lock(&lock);	
	//check if buffer is empty
	while(bcounter == 0){
		printf("\n Consumer says buffer is empty, unable to find product\n");
		pthread_cond_wait(&full, &lock);
	}

	bcounter--;
	NextOut = (NextOut + 1) % BSIZE;
	printf("Consuming item: %d\n", cnum);
	count++;
	pthread_cond_signal(&empty);
	pthread_mutex_unlock(&lock);


	
  }
  myAvg = myTotal/numItems; //calculating consumer average
  printf("\n Consumer values:  avg: %d,  min: %d, max: %d \n ", myAvg, myMin, myMax);

  // put values into total min max and avg calculations
  if(myMin < totMin){
        totMin = myMin;
  }
  else if(myMax > totMax){
        totMax = myMax;
  }
        total += myTotal;

  printf("\n --- Consumer exiting ---\n");

  pthread_exit(0);
}

