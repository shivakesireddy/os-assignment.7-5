#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int accessSeats[2];
int customers[2];
int barber[2];
int freeaccessSeats[2];

void randomWait();
void barber_process();
void customer_process();

void V(int pd[]) {
   int a=1;
   write(pd[1],&a,sizeof(int));
}

void P(int pd[]) {
   int a;
   read(pd[0],&a,sizeof(int));
}

void main() {
   int i;
   pipe(accessSeats);
   pipe(customers);
   pipe(barber);
   pipe(freeaccessSeats); 
	
   V(accessSeats); 
	
   int num = 3; // waiting room seats = 3
   write(freeaccessSeats[1],&num,sizeof(int));
	
   if (fork() == 0) {
      srand(time(0)+1); //Random Seed  
      barber_process();
      return;
   }
   
   for (i = 1; i <= 5; i++) { // five customers
      if (fork() == 0) {
         srand(time(0)+2*i); // different random seed 
         customer_process();
         return;
      }
   }
   sleep(10);
   printf("\ndone\n\n");
}

void barber_process() {
   int i; 
   int num; //number of free seats
   for (i = 1; i <= 10; ++i) {
      printf("Barber %d is trying to get a customer\n",i);
      P(customers); 
      printf("Barber %d is waiting for the seat to become free\n",i);
      P(accessSeats); 
      read(freeaccessSeats[0],&num,sizeof(int));
      num++; 
      write(freeaccessSeats[1],&num,sizeof(int));
      printf("Barber %d is increasing the number of free accessSeats to %d\n",i,num);
      V(barber); 
      V(accessSeats); 
      printf("Barber is now cutting hair %d\n",i);
      randomWait();	
   }
}

void customer_process() {
   int i;
   int num;
   for (i = 1; i <= 2; ++i) {
      printf("New customer trying to find a seat\n");
      P(accessSeats); 
      read(freeaccessSeats[0],&num,sizeof(int));
      if (num > 0) 
      {
         num--;
         write(freeaccessSeats[1],&num,sizeof(int));
         printf("Customer left seat in waiting room. The total free accessSeats are now: %d\n",num);
         V(customers); 
         V(accessSeats); 
         printf("Customer is now waiting for the barber\n");
         P(barber); 
         printf("Customer is now getting a hair cut\n");
      }
      else
      {
         write(freeaccessSeats[1],&num,sizeof(int));
         V(accessSeats); 
         printf("No free chairs in waiting room\n");
      }
      randomWait();
   }
}

void randomWait() { // random delay 
   int delay;
   delay = random() % 9999999999;
   printf("  - wait: %d\n", delay); // debugging - value of wait time
   
}
