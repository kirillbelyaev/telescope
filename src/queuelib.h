/*
 Copyright (c) 2012-2014  Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 * Queue Library support for TeleScope
 */

#ifndef QUEUELIB_H
#define	QUEUELIB_H

#include "globals.h"

#define QUEUE_MAX_ITEMS 1000
#define R_NUM 256

//keeps track of connecting clients
struct clients {
        char ip[256][256];
        int refcount;
};

/*----------------------------------------------------------------------------------------
 * Queue Structure Definition
 * -------------------------------------------------------------------------------------*/
struct QueueStruct {
        // locking for multithreaded environment
        pthread_mutex_t queueLock;

        //   the queue data
        // the index of oldest item
        int head;
        // the index of next available slot
        int tail;
        // an array of messages that are stored in the queue
        char * items[QUEUE_MAX_ITEMS];
};

typedef struct QueueStruct *Queue;

//Queues Table - enables dedicated queue per subscriber
struct QueueTableStruct {
        pthread_mutex_t queueTableLock;
        Queue qtable[R_NUM];
        struct clients clients_tbl;
};

typedef struct QueueTableStruct *QueueTable;

Queue createQueue( void );
void destroyQueue( Queue q );
void clearQueue( Queue q );
bool isQueueEmpty( Queue q );
int writeQueue( Queue q, char *item );
int readQueue( Queue q, char **item );
int sizeQueue( Queue q );
long getItemsUsed( Queue q );
int getItemsTotal( Queue q );
int updateClientsTable( Queue q, const char *peer );
QueueTable createQueueTable( void );
void destroyQueueTable( QueueTable qt );
int updateQueueTableClientsTable( QueueTable qt, const char *peer, int flag );
int decrementQueueTableRefcount( QueueTable qt );
int getRefcount( Queue q );
int getQueueTableRefcount( QueueTable qt );
int getClientAddress( Queue q, int index, char * buff );
int getQueueTableClientAddress( QueueTable qt, int index, char * buff );
int writeQueueTable( QueueTable qt, char * item );

QueueTable getQueueTable();


#endif
