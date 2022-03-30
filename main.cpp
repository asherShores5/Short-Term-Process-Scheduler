//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//Authors: Asher Shores & Philip Varkey
//Assignment: Project 3: Short-Term Process Scheduler
//Instructor: CST-315, Operating Systems Lecture & Lab
//Date: March 29, 2022,
//This our my own work
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <chrono>

using namespace std;

pthread_mutex_t p_control;

//provides the number of threads required in the system to control the processes
#define T_NUM 2
int count;

//main structure used for the processes with the input and position identifying the
//state that the processes is currently in
struct processes
{
public:
  int input;
  string position;
  string executed = "Process executed, yay!!";
};

//Secondary structure that stores the different processes for each state
//with each state being initialized with this structure
struct status
{
  public:
  processes read;
  int val[4];
} idle, wait, ready, run;

//initialize functions
void waitState(status machine);
void readyState(status machine);
void runState(status machine);

int main() {
  srand(time(NULL));

  //Creates the number of threads required to run the system
  pthread_t th[T_NUM];
  pthread_mutex_init(&p_control, NULL);

  //Initializes the processes
  processes pro, pro2, pro3, pro4;

  //Sets the process state
  pro.input = rand()%2+1;
  pro2.input = rand()%2+1;
  pro3.input = rand()%2+1;
  pro4.input = rand()%2+1;

  //adds the processes into the idle queue
  idle.val[0] = pro.input;
  idle.val[1] = pro2.input;
  idle.val[2] = pro3.input;
  idle.val[3] = pro4.input;
  cout << "all processes have been initialized" << endl;
  sleep(1);

  //for loop to iterate over the idle queue
  for (int i = 0; i < 3; i++)
  {
    //if statement to move the states into their proper state
    if (idle.val[i] == 1)
    {
      wait.val[i] = idle.val[i];
      wait.read.position = "wait";
    }
    else if(idle.val[i] == 2)
    {
      ready.val[i] = idle.val[i];
      ready.read.position = "ready";
    }
  }
  //calls on the wait state with the wait queue as the input
  waitState(wait);

  //destroys the thread once the system finishes
  pthread_mutex_destroy(&p_control);
  return 0;
}

void waitState(status machine){

  pthread_mutex_lock(&p_control);
  for (int check = 0; check < 16; check++)
  {
    //if statement to check the value for the initialized processes
    if (machine.val[check] == 3)
    {
        machine.val[check] = 0;
        machine.read.position = "wait";
    }
  }

  pthread_mutex_unlock(&p_control);

  //for loop to iterate over the queue
  for (int reading = 0; reading < 4; reading++)
  {
    //move the values into the ready queue
    if (machine.val[reading] == 1)
    {
      machine.val[reading] = 2;
      ready.val[reading] = machine.val[reading];
      machine.read.position = "ready";

      cout << "moved from the wait state to the ready state" << endl;
      sleep(1);
    }
    else if (machine.val[reading] == 2)
    {
      machine.read.position = "ready";
      ready.val[reading] = machine.val[reading];
      cout << "Already in the ready state" << endl;
      sleep(1);
    }
    else
    {
      ready.val[reading] = 2;
      cout << "moved to the ready state" << endl;
      //reading = reading - 1;
      sleep(1);
    }
  }
  //Call on the ready function
  readyState(ready);
  return;
}

void readyState(status machine){
  //locks the system to check the state for each process
  pthread_mutex_lock(&p_control);

  //for loop to iterate through the running processes
  for (int que = 0; que < 4; que++)
  {
      //if statement evaluates the state for each process and adjusts its location
      if (machine.val[que] == 3)
      {
          machine.val[que] = 1;
          wait.val[que+4] = machine.val[que];
          waitState(machine);
      }
  }
  //unlocks the system once the check is completed
  pthread_mutex_unlock(&p_control);

  //Move the processes into the run queue using the for loop
  for (int change = 0; change < 4; change++)
  {
    if (machine.val[change] == 2)
    {
      //changes the value for the system and moves the process into the run queue
      machine.val[change] = 3;
      run.val[change] = machine.val[change];
      machine.read.position = "run";
      count = count + 1;

      cout << "moved from the ready state to the run state"<< endl;
      sleep(1);
    }
    else if (machine.val[change] == 3)
    {
      //moves the process into the run queue
      machine.read.position = "run";
      run.val[change] = machine.val[change];

      cout << "Already in the run state " << endl;
      sleep(1);
    }
    else
    {
        sleep(1);
    }
  }
  //Call on the run function
  runState(run);
  return;
}

void runState(status machine){
  //Set the initial time for running the system at 5ms
  chrono::steady_clock::time_point begin = chrono::steady_clock::now();

  for (int done = 0; done < count; done++)
    {
        if (machine.val[done] == 3)
        {
            //Add the process back into the wait queue
            machine.val[done] = 1;
            wait.val[done] = machine.val[done];
            machine.read.position = "wait";

            //run each process and output to the user that is has run
            cout << machine.read.executed << endl;
            cout << "Process returned to the wait state" << endl;
            sleep(1);
        }
        else
        {
            //Returns any processes back to the ready state
            machine.val[done] = 2;
            ready.val[done+4] = machine.val[done];
            machine.read.position = "run";
            cout << "process returned to the ready queue" << endl;
        }
        //finds the time for the process to run
        chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

        //if statement to check the total time for the runtime
        if(chrono::duration_cast<chrono::seconds>(end - begin).count() >= 3)
        {
            if (machine.val[done] == 3)
            {
                machine.val[done] = 2;
                ready.val[done + 4] = machine.val[done];
                machine.read.position = "run";
                cout << "process returned to the ready queue" << endl;
            }
        }
    }
  //resets the system and calls on waitState
  count = 0;
  waitState(machine);

  return;
}