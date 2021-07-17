#pragma once
#include "linked_list.h"
#include "disastrOS_resource.h"

typedef struct MessageQueue{
    Resource res;
    ListHead messages;
}MessageQueue;

//Initializes memory allocator for MQs
void MessageQueue_init();

//Allocates memory for a MQ
MessageQueue* MessageQueue_alloc(int id);

//Frees memory for a message queue
int MessageQueue_free(MessageQueue* mq);