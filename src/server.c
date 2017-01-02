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

#include "server.h"

static int threadIDCounter = 0; //counter to keep track of Reader's ID


//socket stuff

//sockets
int server_sock;

pthread_mutex_t threadIDLock;


void setupServer( void );
int prepareServerSocket( void );
void *Server( void *param );

//save the peer address from the incoming client connection
int getSubscriberAddress( const struct sockaddr *addr, char *ip );
int get_threadIDCounter( void );

const xmlChar subscriber_intro[] =
        "<xml><XML_MESSAGE length=\"00000128\" version=\"0.2\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.2\" type_value=\"0\" type=\"MESSAGE\"></XML_MESSAGE>";

int getSubscriberAddress( const struct sockaddr *addr, char *ip )
{
    const unsigned char *rawaddr = NULL;
    char printable[80];
    const char *peer = NULL;

    if ( addr->sa_family == AF_INET6 )
    {
        const struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        rawaddr = (const unsigned char *)&addr6->sin6_addr;
    }
    else if ( addr->sa_family == AF_INET )
    {
        const struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        rawaddr = (const unsigned char *)&addr4->sin_addr.s_addr;
    }
    if ( rawaddr != NULL )
        peer = inet_ntop( addr->sa_family, rawaddr, printable,
            sizeof( printable ) );
    if ( peer == NULL )
        peer = "(unknown)";

    strcpy( ip, peer );
    return 0;
}

int prepareServerSocket( void )
{
    struct sockaddr_in server_sin;
    //build server address data structure
    server_sin.sin_family = AF_INET;
    server_sin.sin_addr.s_addr = INADDR_ANY;
    server_sin.sin_port = htons( startingPort + SERVER_PORT );

    // memset(&server_sin.sin_zero, '\0', sizeof(&server_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

    /* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
    memset( &server_sin.sin_zero, '\0', sizeof( *server_sin.sin_zero ) ); /* Wed May 21 11:33:42 PDT 2014 */

    if ( ( server_sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "can't open a socket!" );
        exit( 1 );
    }

    //lets reuse the socket!
    int v = 1;
    setsockopt( server_sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof( v ) );

    if ( ( bind( server_sock, (struct sockaddr *)&server_sin,
        sizeof( server_sin ) ) ) < 0 )
    {
        perror( "can't bind to a socket!" );
        exit( 1 );
    }

    //lets put the server socket into
    //listening mode to accept client requests
    if ( listen( server_sock, R_NUM ) < 0 )
    {
        perror( "can't start listening on a socket!" );
        exit( 1 );
    }
    return 0;
}

void launchServerThreadPool( void )
{
    int tindex;
    pthread_attr_t attr;
    pthread_t tidR[R_NUM];

    setupServer( );

    prepareServerSocket();

    // Get the default attributes
    pthread_attr_init( &attr );

    memset( tidR, '\0', sizeof(pthread_t) * R_NUM );

    /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */
    for ( tindex = 0; tindex < R_NUM; tindex++ )
    {
        if ( pthread_create( tidR + tindex, &attr, Server, (void *)&server_sock ) != 0 )
        {
            perror( "failed to create a server thread!" );
            exit( -1 );
        }
        if ( pthread_detach( tidR[tindex] ) != 0 )
        {
            perror( "Server thread failed to detach!" );
            exit( -1 );
        }
        usleep(1000); // Attempt to prevent failed thread create.  TODO evaluate effectiveness
    }
    pthread_attr_destroy( &attr );
    fprintf( stdout, "Server Thread Pool started. Listening on port %ld \n",
    startingPort + SERVER_PORT );
    fprintf( logfile, "Server Thread Pool started. Listening on port %ld \n",
    startingPort + SERVER_PORT );
}

void *Server( void *threadid )
{
    int lsock, sock;
    lsock = *( (long*)threadid );
    char *xmlR = NULL;
    char ipstring[256];
    struct sockaddr_in clientName = { 0 };
    socklen_t clientNameLen = sizeof( clientName );

    int myID;
    Queue XMLQ = NULL;

    char filter[8];
    char refilter[9];

    // detach the thread so the resources may be returned when the thread exits
    pthread_detach( pthread_self() );

    myID = get_threadIDCounter(); /* obtain the ID of the queue to read from by brute force */
    XMLQ = getQueueTable()->qtable[myID];
    //XMLQ = XMLQS[myID];

    memset( ipstring, '\0', sizeof( ipstring ) );
    memset( filter, '\0', sizeof( filter ) );
    memset( refilter, '\0', sizeof( refilter ) );

    struct timespec sleeptime;
    sleeptime.tv_sec = 0;
    sleeptime.tv_nsec = 1000;

    while ( terminateFlag == 0 )
    {            //outer loop starts
        if ( ( sock = accept( lsock, (struct sockaddr *)&clientName, &clientNameLen ) ) < 0 )
        { //need this version to get info about subscriber's IP
            perror( "Reader: can't accept on a socket!" );
            close( sock );
            continue;
        }
        else
        {
            getSubscriberAddress( (struct sockaddr *)&clientName, ipstring );
            logMessage( stdout, "Reader: Accepting incoming client from IP: %s\n",
                ipstring );
            //updateClientsTable(XMLQ, ipstring);
            updateQueueTableClientsTable( getQueueTable(), ipstring, 1 );

            //write introductory bgp message with <xml> tag to order the reading correctly in the client
            write( sock, subscriber_intro, strlen( (char *)subscriber_intro ) );
        }
        //inner processing loop
        while ( true )
        {
            if ( isQueueEmpty( XMLQ ) == true )
            {
                // Wait 1ms to prevent 100% CPU utilization.
                usleep(1000);
                continue;
            }
            else
            {
                // Sleep for 1 micro second
                nanosleep( &sleeptime, NULL );
                readQueue( XMLQ, &xmlR );

                if ( xmlR == NULL )
                    continue;

                size_t xlen = strlen( xmlR );

                if ( ( write( sock, xmlR, xlen ) == -1 ) )
                {
                    logMessage( logfile,
                            "Reader: client closed the connection\n" );
                    break; //break out of the inner while loop
                }
                free(xmlR);

            } //end of else when queue is not empty
        } //end of inner while loop
        logMessage( stdout, "Reader: exited the inner while loop!\n" );
        updateQueueTableClientsTable( getQueueTable(), ipstring, 2 );
        close( sock );
    } //end of outer while loop

    pthread_exit( (void *)1 );
} //end of Reader

void setupServer( void )
{
    createQueueTable();
    if ( pthread_mutex_init( &threadIDLock, NULL ) )
    {
        perror( "unable to init mutex ID lock" );
        exit( 1 );
    }
}

int get_threadIDCounter( void )
{
    int t = 0;

    lockMutex( &threadIDLock );

    t = threadIDCounter;
    threadIDCounter++; //JUST update the counter /* Thu May 22 15:36:29 PDT 2014 */

    unlockMutex( &threadIDLock );
    return t;
}
