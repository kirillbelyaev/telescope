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

#include "functions.h"

int peer_sock;

pthread_t subscriberThread;

//XML object structure: in - incoming raw data stream; out - refined xml data object

const xmlChar msg_start[] = "<XML_MESSAGE length=\"00002275";
int start_len_pos = 22;

long long int TotalMessagesReceived = 0;

// This is not declared in the function to keep it off the stack since maximum
// stack size is usually much less than 16MB
char buf[MAX_LINE * MAX_LINE];
char filename[MIN_LINE];

int receive_xml_static_buffer( const char *filename );
void *processStream_static_buffer( void * voidptr_filename );

int launchSubscriberThread( const char * host, const char * port, const char * file )
{
    setupBrokerList( host, port );

    connectToNextBroker( &peer_sock );

    strncpy(filename, file, MIN_LINE);

    if ( pthread_create( &subscriberThread, NULL, processStream_static_buffer, (void *)filename )
            != 0 )
    {
        perror( "failed to create subscriber Thread!" );
        exit( -1 );
    }
    return 0;
}

void *processStream_static_buffer( void * voidptr_filename )
{
    char * file = (char *)voidptr_filename;

    while ( terminateFlag != 1 )
    {
        receive_xml_static_buffer( file );
    }      //end of main loop
    pthread_exit( (void *)1 );
}

int receive_xml_static_buffer( const char *filename )
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    int i, j, rcv;

    rcv = recv( peer_sock, buf, xmlStrlen( msg_start ), MSG_WAITALL );
    if ( rcv != xmlStrlen( msg_start ) )
    {
        if ( rcv < 0 )
        {
            return -1;
        }
        else if ( rcv == 0 )
        {
            logMessage( stdout, "Connection to current socket closed. Attempting to open new connection.\n" );
            connectToNextBroker( &peer_sock );
            return -1;

        }
        else
        {
            logMessage( stderr, "Failed to read %d bytes from socket.\n", xmlStrlen( msg_start));
            return -1;
        }
    }
    buf[rcv] = '\0'; // Terminate the string

    // If we established a new connection, we can get a second message with the initial <xml> flag.
    // If that is the case, we shift over what we have read so far, and read 5 more bytes from
    // the socket connection.
    if ( strncmp(buf, "<xml>", 5) == 0 )
    {
        for( i = 0, j = 5; j < xmlStrlen(msg_start); i++, j++ )
            buf[i] = buf[j];
        recv( peer_sock, buf + xmlStrlen(msg_start) - 5, 5, MSG_WAITALL );
    }

    long mesg_len = strtol( buf + start_len_pos, NULL, 10 );
    if ( ( errno == ERANGE && ( mesg_len == LONG_MAX || mesg_len == LONG_MIN ) )
            || ( errno != 0 && mesg_len == 0 ) || mesg_len > sizeof( buf ) )
    {
        logMessage( stdout, "Failed to get message length\n" );
        return -1;
    }

    rcv = recv( peer_sock, buf + xmlStrlen( msg_start ),
        mesg_len - xmlStrlen( msg_start ), MSG_WAITALL );
    if ( rcv != mesg_len - xmlStrlen( msg_start ) )
    {
        perror( "Failed to read the remainder of the message" );
        return -1;
    }
    buf[mesg_len] = '\0'; // Terminate the string

    // XML Stuff
    doc = xmlParseMemory( buf, mesg_len );
    if ( doc == NULL )
    {
        perror( "receive_xml_static_buffer(): Failed to parse document!" );
        return ( -1 );
    }
    root_element = xmlDocGetRootElement( doc );
    if ( root_element == NULL )
    {
        perror( "receive_xml_static_buffer(): root element of the XML doc is NULL!\n" );
        xmlFreeDoc( doc );
        doc = NULL;
        return ( -1 );
    }

    // Process the message
    analyze( filename, root_element, buf );

    // Cleanup
    xmlFreeDoc( doc );
    doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
    root_element = NULL; /* Wed Nov 19 17:16:58 MST 2014 */
    TotalMessagesReceived++;        //count the number of BGP messages received
    return 0;
}

long long int getTotalMessagesReceived()
{
    return TotalMessagesReceived;
}
