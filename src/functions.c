/*
 Copyright (c) 2012-2017 Kirill Belyaev
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

#include "functions.h"

volatile sig_atomic_t terminateFlag = 0;

struct sigaction handler; //signal handler mainly for SIGPIPE blocking
struct sigaction term_handler; //signal handler for TERM like signals
pthread_mutex_t terminateLock;

//XML object structure: in - incoming raw data stream; out - refined xml data object

int logging_sock = 0;

void setTerminateFlag( int sigtype )
{
    terminateFlag = 1;
}

int setSignals( void )
{
    //lets install process-wide signal handler to avoid exiting on SIGPIPE signal
    memset( &handler, 0, sizeof( handler ) );
    if ( sigaction( SIGPIPE, NULL, &handler ) < 0 )
    {
        perror( "sigaction() failed for SIGPIPE!" );
    }
    else if ( handler.sa_handler == SIG_DFL )
    {
        handler.sa_handler = SIG_IGN;
        if ( sigaction( SIGPIPE, &handler, NULL ) < 0 )
            perror( "sigaction() failed for SIGPIPE!" );

    }
    //Set term handler for SIGTERM like signals arrival
    memset( &term_handler, 0, sizeof( term_handler ) );
    //term_handler.sa_handler = terminate_;//set the corresponding termination routine
    term_handler.sa_handler = setTerminateFlag; //set the corresponding termination routine
    sigemptyset( &term_handler.sa_mask );
    //now add a bunch of signals...
    if ( sigaddset( &term_handler.sa_mask, SIGTERM ) < 0 )
        perror( "sigaddset() failed!" );
    if ( sigaddset( &term_handler.sa_mask, SIGINT ) < 0 )
        perror( "sigaddset() failed!" );
    if ( sigaddset( &term_handler.sa_mask, SIGQUIT ) < 0 )
        perror( "sigaddset() failed!" );

    term_handler.sa_flags = 0;

    if ( sigaction( SIGTERM, &term_handler, 0 ) < 0 )
        perror( "sigaction() failed for SIGTERM!" );
    if ( sigaction( SIGINT, &term_handler, 0 ) < 0 )
        perror( "sigaction() failed for SIGINT!" );
    if ( sigaction( SIGQUIT, &term_handler, 0 ) < 0 )
        perror( "sigaction() failed for SIGQUIT!" );
    //block signals in threads
    //pthread_sigmask ( SIG_BLOCK, &term_handler.sa_mask, NULL );
    return 0;
}

void lockMutex( pthread_mutex_t * m )
{
    if ( pthread_mutex_lock( m ) )
    {
        perror( "Unable to lock pthread_mutex_t" );
        exit( 1 );
    }
}

void unlockMutex( pthread_mutex_t * m )
{
    if ( pthread_mutex_unlock( m ) )
    {
        perror( "Unable to unlock pthread_mutex_t" );
        exit( 1 );
    }
}

void terminate_( int sigtype )
{
    sigset_t OldMask;
    sigprocmask( SIG_BLOCK, &term_handler.sa_mask, &OldMask );
    setTerminateFlag( 1 );
    usleep( 100000 );

    deleteConnectionManager();

    if ( serverflag == 1 )
    {
        destroyQueueTable( getQueueTable() );
        free( getQueueTable() );
    }

    xmlCleanupParser(); //Free the global libxml variables

    sigemptyset( &term_handler.sa_mask );
    memset( &term_handler, 0, sizeof( term_handler ) );
    sigprocmask( SIG_SETMASK, &OldMask, NULL );

    logMessage( stdout,
        "TeleScope received TERM signal. Freed all memory, canceled all threads and now exiting gracefully.\n" );
    fclose( logfile );
    exit( 0 );
}

void print_service_ports( void ) /* Mon May 26 17:15:55 MDT 2014 */
{
    fprintf( stdout,
        "When in server mode TeleScope is listening on the following ports: \n" );
    fprintf( stdout, "Clients Thread Pool - port %ld \n", startingPort + SERVER_PORT );
    fprintf( stdout, "Status Thread - port %ld \n", startingPort + STATUS_PORT );
    fprintf( stdout, "CLI Thread - port %ld \n", startingPort + CLI_PORT );
}

/* trims the string from both sides removing trailing spaces */
int trim( char *str )
{
    int i, l;
    int size = strlen( str );

    if ( size == 0 )
        return -1;

    //remove blanks from the beginning
    for ( i = 0; isspace( str[i] ); i++ );
    if (i)
        for ( l = 0; i <= size; i++, l++ )
            str[l] = str[i];
    else
        l = size + 1;

    //remove blanks from the end
    for ( i = l - 2; i >= 0 && isspace( str[i] ); i-- )
        str[i] = '\0';

    return 0;
}

void showUptime( int sock )
{
    int uptime = difftime(time(NULL), serverStartTime) + 0.5;

    int hours = uptime / 3600;
    int minutes = ( uptime % 3600 ) / 60;
    int seconds = uptime % 60;

    char tmp[MAX_LINE];
    snprintf(tmp, MAX_LINE, "Server uptime: %02d:%02d:%02d\n", hours, minutes, seconds );
    write( sock, tmp, strlen( tmp ) );
}

void logMessage( FILE *stream, const char *format, ... )
{
    char buffer[MAX_LINE];

    va_list args;
    va_start(args, format);
    vsnprintf( buffer, MAX_LINE, format, args );
    va_end( args );
    fprintf( stream, "%s", buffer );
    fprintf( logfile, "%s", buffer );
    if ( logging_sock )
        write( logging_sock, buffer, strlen(buffer) );
}
