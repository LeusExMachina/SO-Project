# SO-Project
Progetto Sistemi Operativi A.A. 2020/2021


The purpose of this project is to add an IPC feature to the user-space OS
disastrOS.

GENERAL SURVEY

I added Message Queues to the base code provided by the professor and seen
during the lessons. These Message Queues allow two or more disastrOS processes
to communicate sending and receiving messages.

The main functions of any Message Queue implementation are read and write
functions. In this case, the write is non-blocking; therefore the process
continues his execution even if the written message is not read (immediately 
or at all).

Otherwise, the read is blocking: if a process tries to read a message without
managing to, it will block until a new message is written by another process
and is received by him.

IMPLEMENTATION DETAILS

I added a class Message. This class inherits from ListItem so that the 
messages can be arranged into a list. To make this class inherit from ListItem,
the first field in the struct Message is a ListItem element. The only other
field is an array of char ad its size is fixed by a system constant.

Each Message object is allocated trough a specific PoolAllocator, so we have
functions to initialize the allocator, to allocate a new message and free
an allocated message. Other functions allow to read and write a string from
and to a message object.

I added then a class MessageQueue. This class inherits from the class Resource,
which is already present in the base code. Resource also inherits from
ListItem, so also MessageQueue objects can be arranged into a list.

In addition to all the parameters of Resource, MessageQueue has another 
parameter that is a list of messages. Each MessageQueue object is allocated
with a specific PoolAllocator, and there are functions to initialize the
allocator, to allocate a new MessageQueue and to free an allocated
MessageQueue.

I modified the functions openResource, closeResource and destroyResource, so 
that if the Resource to create (or destroy) is a MQ, the MQ allocator will
be used. I also created wrappers to alloc, free and destroy a MQ without having
to specify the resource type.

Finally I added the functions MQread and MQwrite. MQread tries to read a
message from a MQ detaching it from the messages list of the MQ. If the
function does not find any message, it will pause the process, putting it in
the waiting list, standing by a message.

MQwrite initially checks if there are processes waiting for a Message in the
waiting list. It also checks if the MQ opened by an eventual waiting process
is the same opened by the process executing the function. If MQwrite finds a 
waiting process respecting these charateristics, it will communicate directly
with that process writing the message. Else, MQwrite will create a new message
object with the content specified in the call and will add it to the messages 
list in the MQ.
 
