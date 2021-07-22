#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "disastrOS.h"
#include "disastrOS_constants.h"

//to remove
/*#include "disastrOS_resource.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_globals.h"*/

#include <string.h>


// we need this to handle the sleep state
void sleeperFunction(void* args){
  printf("Hello, I am the sleeper, and I sleep %d\n",disastrOS_getpid());
  while(1) {
    getc(stdin);
    disastrOS_printStatus();
  }
}

void childFunction(void* args){
  printf("Hello, I am the child function %d\n",disastrOS_getpid());
  printf("I will iterate a bit, before terminating\n");
  int type= 0;
  int mode=0;
  int fd=disastrOS_openResource(disastrOS_getpid(),type,mode);
  printf("fd=%d\n", fd);
  printf("PID: %d, terminating\n", disastrOS_getpid());

  for (int i=0; i<(disastrOS_getpid()+1); ++i){
    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }
  disastrOS_exit(disastrOS_getpid()+1);
}

void producerFunction(void* args){

  char buffer[1024];
  int fd = disastrOS_openMQ(0, DSOS_CREATE);

  for (int i = 0; i < 10; i++){

    printf("-- SONO IL PRODUCER --\n");

    memset(buffer, 0, 1024);
    strcpy(buffer, "sono il producer e comunico con il consumer");

    disastrOS_MQwrite(fd, buffer);

    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }

  disastrOS_closeMQ(fd);

  disastrOS_destroyMQ(fd);

  disastrOS_exit(0);
}

void consumerFunction(void* args){

  char buffer[1024];
  int fd = disastrOS_openMQ(0, DSOS_READ);

  for (int i = 0; i < 10; i++){

    printf("-- SONO IL CONSUMER --\n");

    memset(buffer, 0, 1024);

    disastrOS_MQread(fd, buffer);
    printf("%s\n", buffer);

    printf("PID: %d, iterate %d\n", disastrOS_getpid(), i);
    disastrOS_sleep((20-disastrOS_getpid())*5);
  }

  disastrOS_closeMQ(fd);

  disastrOS_exit(0);
}


void initFunction(void* args) {
  disastrOS_printStatus();
  printf("hello, I am init and I just started\n");
  disastrOS_spawn(sleeperFunction, 0);
  
  //TEST OPEN

  /*int fd = disastrOS_openMQ(0, DSOS_CREATE);
  Resource* res = ResourceList_byId(&resources_list, 0);
  MessageQueue* mq = (MessageQueue*) res;

  ListHead i = mq->messages;


  disastrOS_closeMQ(fd);
  disastrOS_destroyMQ(0);*/

  //TEST WRITE-READ(single process)

  /*int fd = disastrOS_openMQ(0, DSOS_CREATE);
  char test[16] = "ciao come stai";

  char read[1024];
  memset(read, 0, 1024);

  int res1 = disastrOS_MQwrite(fd, test);

  int res2 = disastrOS_MQread(fd, read);

  printf("%s\n", read);

  printf("%d\n", res1);
  printf("%d\n", res2);*/

  printf("I spawn producer and consumer processes\n");

  int alive_children = 0;

  disastrOS_spawn(producerFunction, 0);
  alive_children++;

  disastrOS_spawn(consumerFunction, 0);
  alive_children++;

  disastrOS_printStatus();

  int retval;
  int pid;

  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }

  /*
  printf("I feel like to spawn 10 nice threads\n");
  int alive_children=0;
  for (int i=0; i<10; ++i) {
    int type=0;
    int mode=DSOS_CREATE;
    printf("mode: %d\n", mode);
    printf("opening resource (and creating if necessary)\n");
    int fd=disastrOS_openResource(i,type,mode);
    printf("fd=%d\n", fd);
    disastrOS_spawn(childFunction, 0);
    alive_children++;
  }

  disastrOS_printStatus();
  int retval;
  int pid;
  while(alive_children>0 && (pid=disastrOS_wait(0, &retval))>=0){ 
    disastrOS_printStatus();
    printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
	   pid, retval, alive_children);
    --alive_children;
  }*/

  printf("shutdown!\n");
  disastrOS_shutdown();
}

int main(int argc, char** argv){
  char* logfilename=0;
  if (argc>1) {
    logfilename=argv[1];
  }
  // we create the init process processes
  // the first is in the running variable
  // the others are in the ready queue
  printf("the function pointer is: %p", childFunction);
  // spawn an init process
  printf("start\n");
  disastrOS_start(initFunction, 0, logfilename);
  return 0; 
}