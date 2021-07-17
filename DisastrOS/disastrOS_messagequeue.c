#include <assert.h>
#include <stdio.h>
#include "disastrOS_resource.h"
#include "disastrOS_messagequeue.h"
#include "pool_allocator.h"
#include "linked_list.h"

#define MESSAGEQUEUE_SIZE sizeof(MessageQueue)
#define MESSAGEQUEUE_MEMSIZE (sizeof(MessageQueue) + sizeof(int))
#define MESSAGEQUEUE_BUFFER_SIZE MAX_NUM_MESSAGEQUEUES*MESSAGEQUEUE_MEMSIZE

static char _messagequeues_buffer[MESSAGEQUEUE_BUFFER_SIZE];
static PoolAllocator _messagequeues_allocator;

void MessageQueue_init(){
    int result=PoolAllocator_init(
        & _messagequeues_allocator,
		MESSAGEQUEUE_SIZE,
		MAX_NUM_MESSAGEQUEUES,
		_messagequeues_buffer,
		MESSAGEQUEUE_BUFFER_SIZE);
    assert(! result);
}

MessageQueue* MessageQueue_alloc(int id){
    MessageQueue* mq = PoolAllocator_getBlock(&_messagequeues_allocator);
    if (!mq) return 0;

    Resource* res = (Resource*) mq;
    res->list.prev = res->list.next=0;
    res->id = id;
    res->type = RES_MQ;
    List_init(&res->descriptors_ptrs);

    List_init(&mq->messages);

    return mq;
}


int MessageQueue_free(MessageQueue* mq){

    assert(mq->messages.first == 0);
    assert(mq->messages.last == 0);

    Resource* r = (Resource*) mq;

    assert(r->descriptors_ptrs.first==0);
    assert(r->descriptors_ptrs.last==0);

    return PoolAllocator_releaseBlock(&_messagequeues_allocator, mq);
}