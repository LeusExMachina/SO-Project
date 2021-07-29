#pragma once
#include "linked_list.h"
#include "disastrOS_constants.h"


//Message sent by a process to a Message Queue

typedef struct Message{
    ListItem list;
    char message[MAX_MESSAGE_LEN];
} Message;

//Initializes the memory allocation
//for the Message structure
void Message_init();

//Creates an empty message
Message* Message_alloc();

//Frees a message
int Message_free(Message* msg);

//Writes text written in string in a message object
void Message_write(char* string, Message* msg);

//Reads the message in a message object
//and copies it to the buffer pointed by message
void Message_read(char* string, Message* msg);
