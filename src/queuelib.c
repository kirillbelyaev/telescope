/*
 Copyright (c) 2012-2017 Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 * Queue Library support for TeleScope
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <netinet/in.h>

#include <sys/types.h>

#include <pthread.h>

#include "queuelib.h"

QueueTable QT = NULL; //consolidated data structure

void queueNext( int * i )
{
    *i = ( *i + 1 ) % QUEUE_MAX_ITEMS;
}

void queuePrev( int * i )
{
    *i = ( *i ? *i - 1 : QUEUE_MAX_ITEMS - 1 );
}

Queue createQueue( void )
{
    Queue q = (Queue)malloc( sizeof(struct QueueStruct) );
    if ( q == NULL )
    {
        perror( "out of memory: malloc queue structure failed" );
        exit( 1 );
    }
    memset( q, 0, sizeof(struct QueueStruct) );

    // set queue head and tail to 0, clear all queue items
    q->head = 0;
    q->tail = 0;
    memset( q->items, 0, QUEUE_MAX_ITEMS * sizeof(char *) );

    // create a lock for this queue
    if ( pthread_mutex_init( &q->queueLock, NULL ) )
    {
        perror( "unable to init mutex lock for queue" );
        exit( 1 );
    }

    return ( q );
}

QueueTable createQueueTable( void )
{
    int i;
    if ( QT )
    {
        destroyQueueTable(QT);
        free(QT);
    }
    QT = malloc( sizeof(struct QueueTableStruct) );
    if ( QT == NULL )
    {
        perror( "out of memory: malloc queue table structure failed" );
        exit( 1 );
    }
    memset( QT, 0, sizeof(struct QueueTableStruct) );

    // create a lock for this queue table
    if ( pthread_mutex_init( &QT->queueTableLock, NULL ) )
    {
        perror( "unable to init mutex lock for queue table" );
        exit( 1 );
    }

    for ( i = 0; i < R_NUM; i++ )
        QT->qtable[i] = createQueue();

    //initialize the clients_table
    memset( &QT->clients_tbl, '\0', sizeof( QT->clients_tbl ) );

    return ( QT );
}

void destroyQueue( Queue q )
{
    lockMutex( &q->queueLock );

    int i;
    for ( i = 0; i < QUEUE_MAX_ITEMS; i++ )
    {
        if ( q->items[i] != NULL )
        {
            free( q->items[i] );
            q->items[i] = NULL;
        }
    }
    q->head = 0;
    q->tail = 0;

    unlockMutex( &q->queueLock );
}

void destroyQueueTable( QueueTable qt )
{
    int i;

    lockMutex( &qt->queueTableLock );

    for ( i = 0; i < R_NUM; i++ )
    {
        destroyQueue( qt->qtable[i] );
        qt->qtable[i] = NULL;
    }

    //clear clients data
    memset( &qt->clients_tbl, '\0', sizeof( qt->clients_tbl ) );

    unlockMutex( &qt->queueTableLock );
}

void clearQueue( Queue q )
{
    lockMutex( &q->queueLock );

    q->head = q->tail;

    unlockMutex( &q->queueLock );
}

bool isQueueEmpty( Queue q )
{
    bool retval;

    lockMutex( &q->queueLock );

    retval = q->tail == q->head;

    unlockMutex( &q->queueLock );

    return retval;
}

int writeQueue( Queue q, char *item )
{
    size_t len = 0;
    int retval = 0;

    lockMutex( &q->queueLock );

    len = strlen( item );

    q->items[q->tail] = realloc( q->items[q->tail],  len * sizeof(char) + 1 );
    strcpy(q->items[q->tail], item );
    queueNext( &q->tail );

    if ( q->tail == q->head )
        queueNext( &q->head );

    unlockMutex( &q->queueLock );
    return retval;
}

int sizeQueue( Queue q )
{
    int size;

    lockMutex( &q->queueLock );
    size = (q->tail - q->head + QUEUE_MAX_ITEMS) % QUEUE_MAX_ITEMS;
    unlockMutex( &q->queueLock );
    return size;
}

int readQueue( Queue q, char **item )
{
    char * retItem;
    int retval = 0;
    lockMutex( &q->queueLock );

    if ( q->tail == q->head )
    {
        *item = NULL;
        retval = -1;
    }
    else
    {
        retItem = malloc( (strlen( q->items[q->head] ) + 1) * sizeof(char) );
        strcpy(retItem, q->items[q->head]);

        *item = retItem;
        queueNext( &q->head );
    }

    unlockMutex( &q->queueLock );
    return retval;
}

int updateQueueTableClientsTable( QueueTable qt, const char *peer, int flag )
{
    int i;
    int index = -1;
    lockMutex( &qt->queueTableLock );

    if ( peer != NULL )
    {
        for ( i = 0; i < qt->clients_tbl.refcount; i++ )
        {
            if ( strcmp( qt->clients_tbl.ip[i], peer ) == 0 )
            {
                index = i;        //record the position
                break;
            }
        }

        if ( qt->clients_tbl.refcount < 256 )
        { //check the bounds
            if ( flag == 1 ) //subscriber connecting
            {
                strcpy( qt->clients_tbl.ip[qt->clients_tbl.refcount], peer );
                qt->clients_tbl.refcount++; //increment the reference count
            }
            else if ( flag == 2 ) //subscriber disconnecting
            {
                if ( index != -1 )
                    strcpy( qt->clients_tbl.ip[index], "" ); //hopefully we located the peer IP to remove it
                qt->clients_tbl.refcount--; //decrement the reference count
            }
        }

    }

    //         printf("clients refcount is: %d\n ", qt->clients_tbl.refcount);
    //         for (i = 0; i < qt->clients_tbl.refcount; i++)
    //             printf("clients: %s\n ", qt->clients_tbl.ip[i]);

    unlockMutex( &qt->queueTableLock );
    return 0;
}

int getQueueTableRefcount( QueueTable qt )
{
    int count;
    lockMutex( &qt->queueTableLock );

    count = qt->clients_tbl.refcount;

    unlockMutex( &qt->queueTableLock );
    return count;
}

int decrementQueueTableRefcount( QueueTable qt )
{
    int count;

    lockMutex( &qt->queueTableLock );

    qt->clients_tbl.refcount--; //JUST update the overwritten position /* Thu May 22 15:36:29 PDT 2014 */

    count = qt->clients_tbl.refcount;
    unlockMutex( &qt->queueTableLock );
    return count;
}

int getQueueTableClientAddress( QueueTable qt, int index, char * buff )
{
    char ip[INET_ADDRSTRLEN];
    strcpy( ip, "" );
    memset( ip, '\0', sizeof( ip ) );

    lockMutex( &qt->queueTableLock );
    if ( index >= 0 && index < qt->clients_tbl.refcount )
        strcpy( ip, qt->clients_tbl.ip[index] );
    // unlock the queue
    unlockMutex( &qt->queueTableLock );
    strcpy( buff, ip );
    return 0;
}

int writeQueueTable( QueueTable qt, char * item )
{
    int i;
    if ( qt != NULL )
    {
        for ( i = 0; i < R_NUM; i++ )
            writeQueue( qt->qtable[i], item );

        return 0;
    }
    else
    {
        perror( "Queue Table is not initialized!" );
        return -1;
    }
}

QueueTable getQueueTable()
{
    return QT;
}
