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

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>

#include <pthread.h>

#define QUEUE_MAX_ITEMS 1000

//keeps track of connecting clients
struct clients {
    char ip[256][256];
    int refcount;
    int size;
               };

/* Entries that are stored in the queue 
 * -------------------------------------------------------------------------------------*/
typedef struct QueueEntryStruct
{
        // a reference count    
        int             count;
        // a pointer to the actual message buffer
        void            *messageBuf;
} QueueEntry;

/*----------------------------------------------------------------------------------------
 * Queue Structure Definition
 * -------------------------------------------------------------------------------------*/
struct QueueStruct
{
        // locking for multithreaded environment
        pthread_mutex_t         queueLock;
        pthread_cond_t          queueCond;

        //   the queue data
        // the index of oldest item
        long                    head;
        // the index of next available slot
        long                    tail;
        int             	count;
        // an array of messages that are stored in the queue
        QueueEntry              items[QUEUE_MAX_ITEMS];
	// the data copy function for items in this queue
	void                    (*copy)(void **copy, void *original);
        int                     (*sizeOf)(void *msg);
	// the sizeof function for items in this queue
	struct clients clients_tbl;

};

typedef struct QueueStruct      *Queue;


//Queues Table - enables dedicated queue per subscriber
struct QueueTableStruct {
    pthread_mutex_t         queueTableLock;
    Queue qtable[R_NUM];
    struct clients clients_tbl;
                        };

typedef struct QueueTableStruct      *QueueTable;              
              

Queue createQueue(void);
void destroyQueue(Queue q);
void clearQueue(Queue q);
bool isQueueEmpty( Queue q );
int writeQueue(Queue q, void *item );
int readQueue (Queue q, void **item );
int sizeQueue(Queue q);
long getItemsUsed(Queue q);
int getItemsTotal(Queue q);
int updateClientsTable(Queue q, const char *peer);
QueueTable createQueueTable(void);
void destroyQueueTable(QueueTable qt);
int updateQueueTableClientsTable(QueueTable qt, const char *peer, int flag);
int decrementQueueTableRefcount(QueueTable qt);
int getRefcount(Queue q);
int getQueueTableRefcount(QueueTable qt);
int getClientAddress(Queue q, int index, char * buff);
int getQueueTableClientAddress(QueueTable qt, int index, char * buff);


Queue createQueue(void)
{
	int i;
	Queue q = malloc( sizeof ( struct QueueStruct ) );
        if ( q == NULL)
		{
                perror( "out of memory: malloc queue structure failed");
		exit(1);
		}
        memset( q, 0, sizeof( struct QueueStruct ) );
	
	// set queue head and tail to 0, clear all queue items
        q->head = 0;
        q->tail = 0;
        memset( q->items, 0, QUEUE_MAX_ITEMS*sizeof( QueueEntry ) );

	for (i = 0; i < QUEUE_MAX_ITEMS; i++)	
		q->items[i].messageBuf = NULL;

	// create a lock for this queue 
        if (pthread_mutex_init( &q->queueLock, NULL ) )
		{
                perror( "unable to init mutex lock for queue");
		exit(1);
		}

        // create notify condition for blocked readers
        if (pthread_cond_init( &q->queueCond, NULL ) )
		{
                perror( "unable to create blocked reader notify for queue");
		exit(1);
		}

	 //initialize the clients_table
    //h->clients_tbl = calloc(1, sizeof(struct clients));
    q->clients_tbl.size = 256;
    q->clients_tbl.refcount = 0;
        for (i = 0; i < q->clients_tbl.size; i++)
        {
        strcpy(q->clients_tbl.ip[i], "");
        }
			return( q );
}

QueueTable createQueueTable(void)
{
        Queue q = NULL;
        QueueTable qt = NULL; 
        int i;
	qt = malloc( sizeof ( struct QueueTableStruct ) );
        if ( qt == NULL)
		{
                perror( "out of memory: malloc queue table structure failed");
		exit(1);
		}
        memset( qt, 0, sizeof( struct QueueTableStruct ) );
	
        // create a lock for this queue table
        if (pthread_mutex_init( &qt->queueTableLock, NULL ) )
		{
                perror( "unable to init mutex lock for queue table");
		exit(1);
		}

        
        //clear array
        for (i=0; i < R_NUM; i++)
                qt->qtable[i] = NULL;
    
        for (i=0; i < R_NUM; i++)
        {
                q = createQueue();       
                qt->qtable[i] = q;
                q = NULL;
        }
               
    //initialize the clients_table
    //h->clients_tbl = calloc(1, sizeof(struct clients));
        qt->clients_tbl.size = 256;
        qt->clients_tbl.refcount = 0;
        for (i = 0; i < qt->clients_tbl.size; i++)
        {
                strcpy(qt->clients_tbl.ip[i], "");
        }
			return( qt );
}

void destroyQueue(Queue q)
{
        // free all publication data
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
		exit(1);	
		}

        int i;
        for( i = 0; i < QUEUE_MAX_ITEMS; i++ )
        {
                if(q->items[i].messageBuf != NULL)
                {
                        //perror( "unlockQueue: slot is not null! freeing!");
                        free( q->items[i].messageBuf );
                        q->items[i].messageBuf = NULL;
                }
        }

	//clear clients data
        for (i = 0; i < q->clients_tbl.size; i++)
                                    //perror( "unlockQueue: resetting clients_tbl!");
        memset(q->clients_tbl.ip[i], 0, sizeof(q->clients_tbl.ip[i]));
    	q->clients_tbl.refcount = 0;
    	q->clients_tbl.size = 0;

        if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
		exit(1);	
		}

        // clear the structure as a precaution
        memset(q, 0, sizeof( struct QueueStruct));
}

void destroyQueueTable(QueueTable qt)
{
        int i;
    
        if ( pthread_mutex_lock( &qt->queueTableLock ) )
                {
                perror( "lockQueueTable: failed");
                        exit(1);
                }
        
        for (i=0; i < R_NUM; i++)
                destroyQueue(qt->qtable[i]);  
        //clear array
        for (i=0; i < R_NUM; i++)
        {
                //free(qt->qtable[i]);
                qt->qtable[i] = NULL;
        }
        
	//clear clients data
        for (i = 0; i < qt->clients_tbl.size; i++)
                //perror( "unlockQueue: resetting clients_tbl!");
                memset(qt->clients_tbl.ip[i], 0, sizeof(qt->clients_tbl.ip[i]));
    	qt->clients_tbl.refcount = 0;
    	qt->clients_tbl.size = 0;

        // unlock the queue table
        if ( pthread_mutex_unlock( &qt->queueTableLock ) )
                {
                perror( "unlockQueueTable: failed");
                        exit(1);
                }
        
        // clear the structure as a precaution
        memset(qt, 0, sizeof( struct QueueTableStruct));
}

void clearQueue(Queue q)
{
        // free all publication data
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
		exit(1);	
		}

        int i;
        for( i = 0; i < QUEUE_MAX_ITEMS; i++ )
        {
                if(q->items[i].messageBuf != NULL)
                {
                        //perror( "unlockQueue: slot is not null! freeing!");
                        free( q->items[i].messageBuf );
                        q->items[i].messageBuf = NULL;
                }
        }

        if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
		exit(1);	
		}
}

bool isQueueEmpty( Queue q )
{
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
		exit(1);	
		}

        if( q->tail == q->head )
        {
                if ( pthread_mutex_unlock( &q->queueLock ) )
			{
                        perror( "unlockQueue: failed");
			exit(1);	
			}
                return true;
        }
        else
        {
                if ( pthread_mutex_unlock( &q->queueLock ) )
			{
                        perror( "unlockQueue: failed");
			exit(1);	
			}
                return false;
        }
}

int writeQueue(Queue q, void *item )
{
	 size_t len = 0;
         // long swap = 0; /* intermediary variable Wed May 21 11:16:27 PDT 2014 */
         
        // lock the queue       
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
			exit(1);	
		}
         //if queue is full - we have to overwrite it
        if (( q->tail - q->head) >= QUEUE_MAX_ITEMS )
        {
                perror("cannot append an entry to a full queue! Overwriting the entries!\n");
        
                len = strlen(item);

	//if the slot has been previously used, free it
	if (q->items[q->tail].messageBuf != NULL)
	{
	perror("slot has been previously used!\n");
	free(q->items[q->tail].messageBuf);
	q->items[q->tail].messageBuf = NULL;
	}
                if (len > 0)
		{
	q->items[q->tail].messageBuf = malloc( len*sizeof(u_char) );
        if ( q->items[q->tail].messageBuf == NULL) 
			  {
                perror( "out of memory: malloc copy of queue item failed");
                          q->items[q->tail].messageBuf = NULL;
        		if ( pthread_mutex_unlock( &q->queueLock ) )
				{
                		perror( "unlockQueue: failed");
					exit(1);	
				}
                                return -1;
                          }
        
	memmove(q->items[q->tail].messageBuf, (void *)item, len);
        
         if (q->tail > 0 && ((q->tail - q->head) > 0))//if slot index is positive
         {
                // q->tail = q->tail--; //update the overwritten position /* Wed May 21 11:16:27 PDT 2014 */
             
                /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
                // swap = q->tail--; /* Wed May 21 11:16:27 PDT 2014 */
                // q->tail = swap; /* Wed May 21 11:16:27 PDT 2014 */
             
                
                q->tail--; //JUST update the overwritten position /* Thu May 22 15:36:29 PDT 2014 */
                
                q->tail=(q->tail) % QUEUE_MAX_ITEMS;
                                                      
         } else { //if difference is negative 
                q->tail = 0;//reset the tail
                q->tail=(q->tail) % QUEUE_MAX_ITEMS;
                
                // q->tail = q->tail++; /* Wed May 21 11:16:27 PDT 2014 */
                
                /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
                // swap = q->tail++; /* Wed May 21 11:16:27 PDT 2014 */
                // q->tail = swap; /* Wed May 21 11:16:27 PDT 2014 */
                
                q->tail++; //JUST update the overwritten position /* Thu May 22 15:36:29 PDT 2014 */
                
                q->head = q->tail;
                //q->head = 0;
                //q->head = (q->head+1) % QUEUE_MAX_ITEMS;
                }   
                } else {//if len <= 0
                          q->items[q->tail].messageBuf = NULL;
                       }
        
        if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
			exit(1);	
		}
                        return 0;
                        
        } else if (( q->tail - q->head) < 0 ) //if difference becomes negative
        {
                perror("negative queue index! \n");
                //        fprintf(stdout, "tail is: %ld ", q->tail);
                //        fprintf(stdout, "head is: %ld ", q->head);
                q->head = q->tail;
                q->tail = 0;
                q->tail=(q->tail) % QUEUE_MAX_ITEMS;
                
                if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
			exit(1);	
		}
                        return 0;
          
        } else {//if queue is not full yet
        q->count++;
        q->tail=(q->tail) % QUEUE_MAX_ITEMS;

	len = strlen(item);

	//if the slot has been previously used, free it
	if (q->items[q->tail].messageBuf != NULL)
	{
	perror("slot has been previously used!\n");
	free(q->items[q->tail].messageBuf);
	q->items[q->tail].messageBuf = NULL;
	}
        //printf("len is:%d\n", len);
                if (len > 0)
		{
	q->items[q->tail].messageBuf = malloc( len*sizeof(u_char) );
        if ( q->items[q->tail].messageBuf == NULL) 
			  {
                perror( "out of memory: malloc copy of queue item failed");
                          q->items[q->tail].messageBuf = NULL;
        		if ( pthread_mutex_unlock( &q->queueLock ) )
				{
                		perror( "unlockQueue: failed");
					exit(1);	
				}
                                return -1;
                          }
        
	memmove(q->items[q->tail].messageBuf, (void *)item, len);
        
        // q->tail = q->tail++; /* Wed May 21 11:16:27 PDT 2014 */
        
        /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
           // swap = q->tail++; /* Wed May 21 11:16:27 PDT 2014 */
           // q->tail = swap; /* Wed May 21 11:16:27 PDT 2014 */
           
           q->tail++; //JUST update the overwritten position /* Thu May 22 15:36:29 PDT 2014 */
        
                } else {
                          q->items[q->tail].messageBuf = NULL;
                       }
        }
        
	// unlock the queue
        if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
			exit(1);	
		}
			return 0;
}

int sizeQueue(Queue q)
{
	int size;

        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
			exit(1);	
		}
		size = q->count;
        if ( pthread_mutex_unlock( &q->queueLock ) )
		{
                perror( "unlockQueue: failed");
			exit(1);	
		}
        return size;
}

int readQueue (Queue q, void **item )
{ 
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
		exit(1);	
		}

        if( q->tail == q->head )
        {
                if ( pthread_mutex_unlock( &q->queueLock ) )
			{
                        perror( "unlockQueue: failed");
			exit(1);	
			}
        perror("cannot read an entry from an empty queue!\n");
                clearQueue(q); //here we free all the slots to ensure we did not malloc extra memory previously
		return -1;
        } else {
        q->count--;
        *item = q->items[q->head].messageBuf;
        q->head = (q->head+1) % QUEUE_MAX_ITEMS;
        //printf("readQ: head is:%s\n", q->items[q->head].messageBuf);
        }
                if ( pthread_mutex_unlock( &q->queueLock ) )
			{
                        perror( "unlockQueue: failed");
			exit(1);	
			}
	return 0;
}

long getItemsUsed(Queue q)
{
	long n;
        if ( pthread_mutex_lock( &q->queueLock ) )
		{
                perror( "lockQueue: failed");
		exit(1);	
		}

        if(q == NULL)
        {
                n = 0;
        }
        else
        {
               n =  q->tail - q->head;
        //               fprintf(stdout, "tail is: %ld ", q->tail);
        //               fprintf(stdout, "head is: %ld ", q->head);
        }
                if ( pthread_mutex_unlock( &q->queueLock ) )
			{
                        perror( "unlockQueue: failed");
			exit(1);	
			}
		return n;
}

/*--------------------------------------------------------------------------------------
 * Purpose: Return how many items are allowed by the queue
 * Input: the queue name in string
 * Output: the number of allowed items in the queue
 * Kirill Belyaev @ June 9, 2011
 * -------------------------------------------------------------------------------------*/
int getItemsTotal(Queue q)
{
        if(q == NULL)
        {
                return 0;
        }
        return QUEUE_MAX_ITEMS;
}

int updateClientsTable(Queue q, const char *peer)
{
                int i, duplicate = 0;
if ( pthread_mutex_lock( &q->queueLock ) )
                {
                perror( "lockQueue: failed");
                        exit(1);
                }

        if (peer != NULL) {
                        for (i = 0; i < q->clients_tbl.refcount; i++)
                        {
                        if (strcmp(q->clients_tbl.ip[i], peer) == 0) {
                                        duplicate = 1;//set the duplicate to true
                                        break;
                                                                     }
                        }

                        if (duplicate == 0) {
                                if (q->clients_tbl.refcount < q->clients_tbl.size) {//check the bounds
                                strcpy(q->clients_tbl.ip[q->clients_tbl.refcount], peer);
                                q->clients_tbl.refcount++;//update the reference count
                                                                                   }
                                            }
                          }

                                printf("clients refcount is: %d\n ", q->clients_tbl.refcount);
                        for (i = 0; i < q->clients_tbl.refcount; i++)
                                printf("clients: %s\n ", q->clients_tbl.ip[i]);
	// unlock the queue
        if ( pthread_mutex_unlock( &q->queueLock ) )
                {
                perror( "unlockQueue: failed");
                        exit(1);
                }
                        return 0;
}

int updateQueueTableClientsTable(QueueTable qt, const char *peer, int flag)
{
        int i; int index = -1;
        if ( pthread_mutex_lock( &qt->queueTableLock ) )
                {
                perror( "lockQueueTable: failed");
                        exit(1);
                }

        if (peer != NULL) 
        {
             for (i = 0; i < qt->clients_tbl.refcount; i++)
                        {
                        if (strcmp(qt->clients_tbl.ip[i], peer) == 0) {
                                        index = i;//record the position
                                        break;
                                                                      }
                        }

              
        if (qt->clients_tbl.refcount < qt->clients_tbl.size) {//check the bounds
                            if (flag == 1) //subscriber connecting
                            {
                                strcpy(qt->clients_tbl.ip[qt->clients_tbl.refcount], peer);
                                qt->clients_tbl.refcount++;//increment the reference count
                            } else if (flag == 2) //subscriber disconnecting
                            {
                                if (index != -1)
                                        strcpy(qt->clients_tbl.ip[index], "");//hopefully we located the peer IP to remove it
                                qt->clients_tbl.refcount--;//decrement the reference count
                            }    
                                                             }
                                  
         }

//         printf("clients refcount is: %d\n ", qt->clients_tbl.refcount);
//         for (i = 0; i < qt->clients_tbl.refcount; i++)
//             printf("clients: %s\n ", qt->clients_tbl.ip[i]);
	
        // unlock the queue table
        if ( pthread_mutex_unlock( &qt->queueTableLock ) )
                {
                perror( "unlockQueueTable: failed");
                        exit(1);
                }
                        return 0;
}


int PrintClientsTable(Queue q)
{
                int i;
        if ( pthread_mutex_lock( &q->queueLock ) )
                {
                perror( "lockQueue: failed");
                        exit(1);
                }
                                printf("clients refcount is: %d\n ", q->clients_tbl.refcount);
                        for (i = 0; i < q->clients_tbl.refcount; i++)
                                printf("clients: %s\n ", q->clients_tbl.ip[i]);
	// unlock the queue
        if ( pthread_mutex_unlock( &q->queueLock ) )
                {
                perror( "unlockQueue: failed");
                        exit(1);
                }
                        return 0;
}

int getRefcount(Queue q)
{
    int count;
        if ( pthread_mutex_lock( &q->queueLock ) )
                {
                perror( "lockQueue: failed");
                        exit(1);
                }
    
        count = q->clients_tbl.refcount;
        
	// unlock the queue
        if ( pthread_mutex_unlock( &q->queueLock ) )
                {
                perror( "unlockQueue: failed");
                        exit(1);
                }
                        return count;
}

int getQueueTableRefcount(QueueTable qt)
{
    int count;
        if ( pthread_mutex_lock( &qt->queueTableLock ) )
                {
                perror( "lockQueueTable: failed");
                        exit(1);
                }
    
        count = qt->clients_tbl.refcount;
        
	// unlock the queue
        if ( pthread_mutex_unlock( &qt->queueTableLock ) )
                {
                perror( "unlockQueueTable: failed");
                        exit(1);
                }
                        return count;
}


int decrementQueueTableRefcount(QueueTable qt)
{
    int count;
    // int swap = 0; /* intermediary variable Wed May 21 11:16:27 PDT 2014 */
    
        if ( pthread_mutex_lock( &qt->queueTableLock ) )
                {
                perror( "lockQueueTable: failed");
                        exit(1);
                }
        
           // qt->clients_tbl.refcount = qt->clients_tbl.refcount--; /* Wed May 21 11:16:27 PDT 2014 */
    
           /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
           // swap = qt->clients_tbl.refcount--; /* Wed May 21 11:16:27 PDT 2014 */
           // qt->clients_tbl.refcount = swap; /* Wed May 21 11:16:27 PDT 2014 */
                
           qt->clients_tbl.refcount--; //JUST update the overwritten position /* Thu May 22 15:36:29 PDT 2014 */
        
        count = qt->clients_tbl.refcount;
	// unlock the queue
        if ( pthread_mutex_unlock( &qt->queueTableLock ) )
                {
                perror( "unlockQueueTable: failed");
                        exit(1);
                }
        return count;
}


int getClientAddress(Queue q, int index, char * buff)
{
                char ip[INET_ADDRSTRLEN];
                strcpy(ip, ""); 
                memset(ip, '\0', sizeof(ip));
                
if ( pthread_mutex_lock( &q->queueLock ) )
                {
                perror( "lockQueue: failed");
                        exit(1);
                }
                if (index >= 0 && index < q->clients_tbl.refcount)
                                strcpy (ip, q->clients_tbl.ip[index]);
	// unlock the queue
        if ( pthread_mutex_unlock( &q->queueLock ) )
                {
                perror( "unlockQueue: failed");
                        exit(1);
                }
                strcpy (buff, ip);
                return 0;
}

int getQueueTableClientAddress(QueueTable qt, int index, char * buff)
{
                char ip[INET_ADDRSTRLEN];
                strcpy(ip, ""); 
                memset(ip, '\0', sizeof(ip));
                
if ( pthread_mutex_lock( &qt->queueTableLock ) )
                {
                perror( "lockQueueTable: failed");
                        exit(1);
                }
                if (index >= 0 && index < qt->clients_tbl.refcount)
                                strcpy (ip, qt->clients_tbl.ip[index]);
	// unlock the queue
        if ( pthread_mutex_unlock( &qt->queueTableLock ) )
                {
                perror( "unlockQueueTable: failed");
                        exit(1);
                }
                strcpy (buff, ip);
                return 0;
}

#endif
