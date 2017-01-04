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

#include "fileReader.h"

char DATAfilename[MIN_LINE];
pthread_t FileReaderThreadID = 0;
char * readbuffer = NULL;
size_t maxReadSize = 0;

void *fileReader( void *param );
int readDataFile( FILE * fd );

void launchFileReaderThread( char * dfn )
{
    strcpy( DATAfilename, dfn );
    //Launch the FileWriter Thread
    // the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed.
    if ( pthread_create( &FileReaderThreadID, NULL, fileReader, NULL ) != 0 )
    {
        perror( "failed to create a File Reader Thread!" );
        exit( -1 );
    }
}

void *fileReader( void *threadid )
{
    FILE * fd;

    logMessage( stdout, "FileReader Thread started. Reading xml data from %s\n",
        DATAfilename );

    // detach the thread so the resources may be returned when the thread exits
    pthread_detach( pthread_self() );

    while ( terminateFlag == 0 )
    {   //outer loop starts
        if ( ( fd = fopen( DATAfilename, "r" ) ) == NULL )
        {
            perror( "fopen failed." );
            exit( -1 );
        }
        else break;
    }

    while ( terminateFlag == 0 )
    {   //outer loop starts
        if ( -2 == readDataFile( fd ) )
            break;
    }
    free( readbuffer );
    fclose( fd );
    pthread_exit( (void *)1 );
}

int readDataFile( FILE * fd )
{
    xmlDoc *doc = NULL;


    if ( getline( &readbuffer, &maxReadSize, fd ) < 0 )
    {
        logMessage( stderr, "Reached end of data file\n" );
        return ( -2 );
    }

    //check for valid input
    if ( strncmp( (char*)msg_start, readbuffer, start_len_pos ) != 0 )
    {
        logMessage( stderr, "Invalid start of xml message\n" );
        return -1;
    }
    trim( readbuffer );

    long mesg_len = strtol( readbuffer + start_len_pos, NULL, 10 );
    if ( ( errno == ERANGE && ( mesg_len == LONG_MAX || mesg_len == LONG_MIN ) )
            || ( errno != 0 && mesg_len == 0 ) || mesg_len != strlen( readbuffer ) )
    {
        logMessage( stderr, "Invalid message length\n" );
        return -1;
    }


    doc = xmlParseMemory( readbuffer, mesg_len ); //changed size to msg_len instead of size_of(): Thu Jun 19 14:53:13 MDT 2014

    if ( doc != NULL )
        writeQueueTable( getQueueTable(), readbuffer );

    // Cleanup
    xmlFreeDoc( doc );
    doc = NULL;

    return 0;
}
