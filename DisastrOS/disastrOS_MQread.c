#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_constants.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

#include "disastrOS_messagequeue.h"
#include "disastrOS_message.h"

void internal_MQread(){
    int fd = running->syscall_args[0];
    char* buffer = (char*) running->syscall_args[1];

    Descriptor* des=DescriptorList_byFd(&running->descriptors, fd);
    if (! des){
        running->syscall_retvalue=DSOS_ERESOURCECLOSE;
        return;
    }

    Resource* res = des->resource;

    if (res->type != RES_MQ){
        running->syscall_retvalue=DSOS_ERESOURCENOMQ;
        return;
    }

    MessageQueue* mq = (MessageQueue*) res;

    if (mq->messages.first == 0){

        running->status = Waiting;
        List_insert(&waiting_list, waiting_list.last, (ListItem*) running);

        PCB* next_running= (PCB*) List_detach(&ready_list, ready_list.first);
        running = next_running;
        return;
    }
    else{
        Message* msg = (Message*) List_detach(&mq->messages, mq->messages.first);
        Message_read(buffer, msg);

        Message_free(msg);

        running->syscall_retvalue = 0;
    }
}