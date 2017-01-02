/*
 Copyright (c) 2012-2014  Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

/*
 "DION
 The violent carriage of it
 Will clear or end the business: when the oracle,
 Thus by Apollo's great divine seal'd up,
 Shall the contents discover, something rare
 Even then will rush to knowledge. Go: fresh horses!
 And gracious be the issue!"

 Winter's Tale, Act 3, Scene 1. William Shakespeare
 */

#include "status.h"

int status_sock;
pthread_t StatusThreadID = 0;

int prepareStatusThread( void );
void *StatusThread( void *threadid );

void launchStatusThread( void )
{
    status_sock = prepareStatusThread();
    //Launch the Status Thread
    /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */
    if ( pthread_create( &StatusThreadID, NULL, StatusThread, (void *)&status_sock )
            != 0 )
    {
        perror( "failed to create a Status Thread!" );
        exit( -1 );
    }
}

//sockets
int prepareStatusThread( void )
{
    struct sockaddr_in status_sin;
    int x = 1, sock;
    //build status address data structure
    status_sin.sin_family = AF_INET;
    status_sin.sin_addr.s_addr = INADDR_ANY;
    status_sin.sin_port = htons( startingPort + STATUS_PORT );

    // memset(&status_sin.sin_zero, '\0', sizeof(&status_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

    /* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
    memset( &status_sin.sin_zero, '\0', sizeof( *status_sin.sin_zero ) ); /* Wed May 21 11:33:42 PDT 2014 */

    if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "can't open a Status socket!" );
        exit( 1 );
    }
    //lets reuse the socket!
    setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, &x, sizeof( x ) );

    if ( ( bind( sock, (struct sockaddr *)&status_sin,
        sizeof( status_sin ) ) ) < 0 )
    {
        perror( "can't bind to a Status socket!" );
        exit( 1 );
    }

    //lets put the Status socket into listening mode to accept Status requests
    if ( listen( sock, 1 ) < 0 )
    {
        perror( "can't start listening on a Status socket!" );
        exit( 1 );
    }
    return sock;
}

/** simplified version since most of the functionality will be moved to cli */
void *StatusThread( void *threadid )
{
    int lsock, sock;
    float ratio = 0;
    lsock = *( (int *)threadid );
    char value[256];
    int i, clientcount;

    logMessage( stdout, "Status Thread started. Listening on port %ld \n",
    startingPort + STATUS_PORT );

    // detach the thread so the resources may be returned when the thread exits
    pthread_detach( pthread_self() );

    while ( terminateFlag == 0 )
    { //outer loop starts
        //if ((sock = accept(lsock, (struct sockaddr *)&server_sin, &len)) < 0 )
        if ( ( sock = accept( lsock, NULL, NULL ) ) < 0 )
        {
            perror( "Status Thread: can't accept on a socket!" );
            close( sock );
            continue;
        }
        else
        {

            write( sock, (char *)"STATUS DATA START:\n",
                strlen( (char *)"STATUS DATA START:\n" ) );

            sprintf( value, (char *)"%lld", getTotalMessagesReceived() );
            write( sock, "TotalMessagesReceived:\n",
                strlen( (char *)"TotalMessagesReceived:\n" ) );
            write( sock, value, strlen( value ) );
            write( sock, "\n", strlen( (char *)"\n" ) );
            memset( value, '\0', sizeof( value ) );

            sprintf( value, (char *)"%lld", getMatchingMessages() );
            write( sock, "MatchingMessages:\n",
                strlen( (char *)"MatchingMessages:\n" ) );
            write( sock, value, strlen( value ) );
            write( sock, "\n", strlen( (char *)"\n" ) );
            memset( value, '\0', sizeof( value ) );

            ratio = ( (double)getMatchingMessages() / (double)getTotalMessagesReceived() )
                    * 100;

            sprintf( value, (char *)"%f", ratio );
            write( sock, "Ratio %:\n", strlen( (char *)"Ratio %:\n" ) );
            write( sock, value, strlen( value ) );
            write( sock, "\n", strlen( (char *)"\n" ) );
            memset( value, '\0', sizeof( value ) );

            clientcount = getQueueTableRefcount( getQueueTable() );

            if ( clientcount > 0 )
            {

                write( sock, (char *)"Number of connected clients is:\n",
                    strlen( (char *)"Number of connected clients is:\n" ) );
                sprintf( value, (char *)"%d", clientcount );
                write( sock, value, strlen( value ) );
                write( sock, "\n", strlen( (char *)"\n" ) );
                memset( value, '\0', sizeof( value ) );

                write( sock, "Client IPs are:\n",
                    strlen( (char *)"Client IPs are:\n" ) );
                for ( i = 0; i < clientcount; i++ )
                {
                    char ip[INET_ADDRSTRLEN];
                    memset( ip, '\0', sizeof( ip ) );
                    getQueueTableClientAddress( getQueueTable(), i, ip );
                    write( sock, ip, strlen( (char *)ip ) );
                    write( sock, "\n", strlen( (char *)"\n" ) );
                    memset( ip, '\0', sizeof( ip ) );

                }
            }
            showUptime(sock);

            write( sock, (char *)"STATUS DATA END:\n",
                strlen( (char *)"STATUS DATA END:\n" ) );
            close( sock );
        }
    }    //end of outer while loop
    pthread_exit( (void *)1 );
}    //end of StatusThread
