

#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>
#include <stdio.h>			//Input Output
#include <iostream>
using namespace std;

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

/*TODO

//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.

 //Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.

 //hint: use sem_t and pthread_mutex_t


 */
sem_t taS; //a semaphore to signal and wait TA's sleep
sem_t chairsS[3]; //array of 3 Semaphores
sem_t studentS; //semaphore for TA's next student

pthread_mutex_t chairs;


//Declared Functions
void *TA_Activity(void *temp);
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

    /*TODO
	//Initializing Mutex Lock and Semaphores.

     //hint: use sem_init() and pthread_mutex_init()

     */

  //**initializing semaphores**
  sem_init(&taS, 0, 0);

  for(id = 0; id < 3; id++)
  {
    sem_init(&chairsS[id], 0, 0);
  }

  sem_init(&studentS, 0, 0);

  //**initializing mutex lock**
  pthread_mutex_init(&chairs, NULL);

	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		number_of_students = atoi(argv[1]);
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);

	}

	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

    /*TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create

	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join

     */

	//**creating TA thread**
	pthread_create(&TA, NULL, TA_Activity, NULL);
  //**creating N student threads**
  for(id = 0; id < number_of_students; id++)
  {
    pthread_create(&Students[id], NULL, Student_Activity, (void*) (long)id);
  }

	//**waiting for TA thread**
	pthread_join(TA, NULL);
  //**waiting for N student threads**
  for(id = 0; id < number_of_students; id++)
  {
    pthread_join(Students[id], NULL);
  }


	//Free allocated memory
	free(Students);
	return 0;
}


void *TA_Activity(void *temp)
{
    /* TODO
	//TA is currently sleeping.

    // lock

    //if chairs are empty, break the loop.

	//TA gets next student on chair.

    //unlock

	//TA is currently helping the student

     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()

*/
    while(true)
    {
      //**TA is currently sleeping**
      sem_wait(&taS); //TA waits to be waken up by student
      printf("Student comes in... \n");
      printf("TA wakes up by the student. \n");

			while(1)
			{
				//**lock**
	      pthread_mutex_lock(&chairs); //locks the preoccupied chairs
	      //**if chairs are empty, break the loop**
	      if(ChairsCount == 0)
	      {
	        pthread_mutex_unlock(&chairs);
	        break;
	      }
				cout << "Chair count before decrement: " << ChairsCount << endl;
	      //**TA gets next student on chair**
	      sem_post(&chairsS[CurrentIndex]); //signals chair that is occupied
	      ChairsCount--; //count decrements when the chairs are occupied

				cout << "Chair count after decrement: " << ChairsCount << endl;

				printf("Student left his/her chair from the waiting room and goes to TA. Remaining chairs: %d\n", 3 - ChairsCount);
	      CurrentIndex = (CurrentIndex + 1) % 3; //increments through the students on the chairs

	      //**unlock chair access**
	      pthread_mutex_unlock(&chairs);

	      //**TA is currently helping the student**
			  printf("TA is currently helping the student. \n");
	      sleep(5); //makes TA thread inactive while he/she helps the student
	      sem_post(&studentS); //releases the student when finished helping and brings in next student
			}

    }

}

void *Student_Activity(void *threadID)
{
    /*TODO

	//Student  needs help from the TA
	//Student tried to sit on a chair.
	//wake up the TA.
    // lock

	// unlock
    //Student leaves his/her chair.
	//Student  is getting help from the TA
	//Student waits to go next.
	//Student left TA room

    //If student didn't find any chair to sit on.
    //Student will return at another time

     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()

	*/

		int TAtime; //time the student spends with TA


		while(true)
		{
			printf("Student %ld is doing the assignment.\n", (long)threadID);
			//assume that each student takes 10 amount of time w/ TA
			TAtime = rand() % 10 + 1;
			sleep(TAtime); //makes student/TA inactive for that amount of time

			//**Students needs help from the TA**
			printf("Student %ld needs help from TA.\n", (long)threadID);
			//**lock**
			pthread_mutex_lock(&chairs);
			//**unlock**
			pthread_mutex_unlock(&chairs);


			//**Student tried to sit on a chair**
			if(ChairsCount < 3) //if the chair count < 3, students can come sit on them
			{
				cout << "There are chair availible." << endl;
				if(ChairsCount == 0) //if the first student is sitting on the first chair
				{
					//**wake up the TA**
					cout << "Student "<< (long)threadID << " trying to Waking up TA.." << endl;
					sem_post(&taS);
				}
				else //if the students are not on the first chair
				{
					printf("Student %ld waits on the chair for the TA to finish.\n", (long)threadID);
				}
				pthread_mutex_lock(&chairs); //locks the preoccupied chairs
				int index = (CurrentIndex + ChairsCount) % 3;
				cout << "The index is: " << index << endl;

				ChairsCount++; //chair count increases when students unoccupy them
				printf("Student sat on the chair. Chairs remaining: %d\n", 3-ChairsCount );
				CurrentIndex = (CurrentIndex + 1) % 3;

				//unlocks chair access
				pthread_mutex_unlock(&chairs);
				sem_wait(&chairsS[index]); //student leaves his/her chair
				//**Student is getting help from the TA**
				printf("Student %ld is getting help from the TA.\n", (long)threadID);
				//**Student waits to go next**
				sem_wait(&studentS);
				//**Student left TA room**
				printf("Student %ld left TA room.\n", (long)threadID);
			}
			//**If student didn't find any chairs to sit on,
			//student will return at another time**
			else
			{
				printf("All the chairs are full. Student %ld will return at another time.\n", (long)threadID);
			}

		}
}
