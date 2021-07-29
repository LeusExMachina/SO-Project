#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_constants.h"
#include "disastrOS_resource.h"
#include "disastrOS_descriptor.h"

#include "disastrOS_messagequeue.h"
#include "disastrOS_message.h"

void internal_MQwrite(){
    int fd = running->syscall_args[0];
    char* buffer = (char*) running->syscall_args[1];
    
    if (strlen(buffer) > MAX_MESSAGE_LEN){
		running->syscall_retvalue=DSOS_OUTOFBOUNDS;
        return;
	}

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

    PCB* waiting_process = 0;
    ListItem* aux = waiting_list.first;

    while(aux){

        waiting_process = (PCB*) aux;

        if (waiting_process->syscall_num == DSOS_CALL_MQREAD){

            int waiting_fd = waiting_process->syscall_args[0];

            Descriptor* wait_des = DescriptorList_byFd(&waiting_process->descriptors, waiting_fd);
            Resource* wait_res = wait_des->resource;

            if (wait_res == res){
                List_detach(&waiting_list, (ListItem*) waiting_process);

                char* wait_buf = (char*) waiting_process->syscall_args[1];

                waiting_process->status = Ready;
                waiting_process->syscall_retvalue = 0;

                strcpy(wait_buf, buffer);

                List_insert(&ready_list, ready_list.last, (ListItem*) waiting_process);

                running->syscall_retvalue = 0;
                return;
            }
        }

        aux = aux->next;
    }

    Message* msg = Message_alloc();
    Message_write(buffer, msg);

    List_insert(&mq->messages, mq->messages.last, (ListItem*) msg);

    running->syscall_retvalue = 0;
    return;

}
