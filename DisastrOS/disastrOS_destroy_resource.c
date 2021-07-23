#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"
#include "disastrOS_messagequeue.h"
#include "disastrOS_message.h"

void internal_destroyResource(){
  int id=running->syscall_args[0];

  // find the resource in with the id
  Resource* res=ResourceList_byId(&resources_list, id);
  if (! res){
    running->syscall_retvalue=DSOS_ERESOURCECLOSE;
    return;
  }

  // ensure the resource is not used by any process
  if(res->descriptors_ptrs.size){
    running->syscall_retvalue=DSOS_ERESOURCEINUSE;
    return;
  }

  res=(Resource*) List_detach(&resources_list, (ListItem*) res);
  assert(res);
  if (res->type == RES_MQ){

    MessageQueue* mq = (MessageQueue*) res;
    
    while(mq->messages.first != 0){
      ListItem* item = List_detach(&mq->messages, mq->messages.first);
      Message* msg = (Message*) item;

      Message_free(msg);
    }

    MessageQueue_free((MessageQueue*) res);
  }
  else{
    Resource_free(res);
  }
  running->syscall_retvalue=0;
}
