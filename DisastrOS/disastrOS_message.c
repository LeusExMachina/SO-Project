#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pool_allocator.h"
#include "disastrOS_message.h"
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include "disastrOS_timer.h"

#define MESSAGE_SIZE sizeof(Message)
#define MESSAGE_MEMSIZE (sizeof(Message) + sizeof(int))
#define MESSAGE_BUFFER_SIZE MAX_NUM_MESSAGES*MESSAGE_MEMSIZE

static char _message_buffer[MESSAGE_BUFFER_SIZE];
static PoolAllocator _message_allocator;

void Message_init(){
    int result=PoolAllocator_init(& _message_allocator,
				  MESSAGE_SIZE,
				  MAX_NUM_MESSAGES,
				  _message_buffer,
				  MESSAGE_BUFFER_SIZE);
    assert(! result);
}

Message* Message_alloc(){
    Message* res = PoolAllocator_getBlock(&_message_allocator);

    res->list.prev = 0;
    res->list.next = 0;

    memset(res->message, 0, MAX_MESSAGE_LEN);

    return res;
}

int Message_free(Message* msg){
    return PoolAllocator_releaseBlock(&_message_allocator, msg);
}

void Message_write(char* string, Message* msg){
    strcpy(msg->message, string);
}

void Message_read(char* string, Message* msg){
    strcpy(string, msg->message);
}