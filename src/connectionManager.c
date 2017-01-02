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

#include "connectionManager.h"

struct Broker {
  char ip[MIN_LINE];
  char port[MIN_LINE];
  char name[MIN_LINE];
  char status[MIN_LINE];
  long priority;
};

struct Broker * broker;
int numBrokers = 0;
int currentBroker = 0;

int addBrokerImpl( char * buffer, int source );
void sortBrokers();
void readTopology();

void addFaultRecord( const char * msg, struct Broker * b );
char ** faultHistory = NULL;
int faultCount = 0;

void setupBrokerList( const char * hvalue, const char * port )
{
    // If we have an existing broker list, delete it.
    if ( broker )
    {
        free(broker);
        broker = NULL;
    }
    numBrokers = 0;
    currentBroker = 0;

    if ( hvalue && port )
    {
        broker = realloc( broker, sizeof( struct Broker ) * ( numBrokers + 1 ) );
        memset(broker + numBrokers, '\0', sizeof( struct Broker) );

        strncpy( broker[numBrokers].ip, hvalue, MIN_LINE );
        strncpy( broker[numBrokers].port, port, MIN_LINE );
        strncpy( broker[numBrokers].name, hvalue, MIN_LINE );
        broker[numBrokers].priority = 100;
        numBrokers++;
    }

    readTopology();
}

void readTopology()
{
    long readSize;
    char * readBuffer = NULL;
    size_t len = 0;
    FILE * fd = fopen( TOPOLOGY_CNF, "r" );

    if ( ! fd ) {
        logMessage(stderr, "Unable to read topology file.\n" );
        return;
    }

    // Read TOPOLOGY_CNF and populate broker list.
    int lineNum = 1;
    while ( ( readSize = getline( &readBuffer, &len, fd ) ) > 0 )
    {
        addBrokerImpl( readBuffer, lineNum );
        lineNum++;
    }

    sortBrokers();

    // Cleanup
    free( (void*)readBuffer );
}

void deleteConnectionManager()
{
    int i;

    free( broker );

    // free fault history
    for ( i = 0; i < faultCount; i++ )
    {
        free( faultHistory[i] );
    }
    free( faultHistory );
}

int failBrokerAdd( const char * attribute, int source )
{
    if ( source > 0 )
        logMessage( stderr, "Invalid %s in topology config file. line %d\n", attribute, source );
    else
        logMessage( stderr, "Invalid %s in broker specification.\n", attribute );
    broker = realloc( broker, sizeof( struct Broker ) * ( numBrokers ) );
    return 1;
}

int addBrokerImpl( char * buffer, int source )
{
    int i;
    char *str, * token, *endNum;
    broker = realloc( broker, sizeof( struct Broker ) * ( numBrokers + 1 ) );
    memset(broker + numBrokers, '\0', sizeof( struct Broker) );

    int error = 0;
    for ( i = 0, str = buffer; error == 0 ; i++, str = NULL )
    {
        token = strtok( str, ";");
        if ( token == NULL )
            break;
        trim(token);
        switch (i)
        {
            case 0:
                if ( strlen( token ) > MIN_LINE-1 )
                    error = failBrokerAdd( "ip", source );
                else
                    strncpy( broker[numBrokers].ip, token, MIN_LINE );
                break;
            case 1:
                if ( strlen( token ) > MIN_LINE-1 )
                    error = failBrokerAdd( "port", source );
                else
                    strncpy( broker[numBrokers].port, token, MIN_LINE );
                break;
            case 2:
                if ( strlen( token ) > MIN_LINE-1 )
                {
                    error = failBrokerAdd( "server name", source );
                }
                else
                    strncpy( broker[numBrokers].name, token, MIN_LINE );
                break;
            case 3:
                broker[numBrokers].priority = strtol( token, &endNum, 10 );
                if ( ( errno == ERANGE && ( broker[numBrokers].priority == LONG_MAX || broker[numBrokers].priority == LONG_MIN ) )
                        || ( errno != 0 && broker[numBrokers].priority == 0 )
                        || *endNum != '\0'
                                || broker[numBrokers].priority > 100
                                || broker[numBrokers].priority < 0  )
                    error = failBrokerAdd( "priority", source );
                break;
        }
    }
    if ( ! error && i != 4 )
        failBrokerAdd("specification", source);

    else
    {
        // Check for duplicate broker names
        for ( i=0; i < numBrokers; i++ )
            if ( strcmp( broker[i].name, broker[numBrokers].name ) == 0 )
                error = failBrokerAdd("duplicate name", source);

        if ( ! error )
        {
            numBrokers++;
            return 1;
        }
    }
    return 0;
}

int addBroker( char * buffer )
{
    int result = addBrokerImpl( buffer, -1 );
    if (result)
        sortBrokers();
    return result;
}

void sortBrokers()
{
    // Sort broker list by priority
    // Simple in-place selection sort since these lists will not be too big,
    // and it is easy to implement, and clear
    struct Broker tmp;
    int pos, searchIndex, emptySlot;
    for ( pos = 0; pos < numBrokers; pos++ )
    {
        memcpy( &tmp, broker + pos, sizeof( struct Broker) );
        emptySlot = 0;
        for ( searchIndex = pos + 1; searchIndex < numBrokers; searchIndex++ )
            if ( broker[searchIndex].priority > broker[pos].priority )
            {
                // Copy the higher priority broker into pos
                memcpy( broker + pos, broker + searchIndex, sizeof( struct Broker) );
                emptySlot = searchIndex;
            }
        // If a higher priority broker was found, we complete
        // the swap by writing tmp into the now empty slot.
        if ( emptySlot )
            memcpy( broker + emptySlot, &tmp, sizeof( struct Broker) );
    }
}

int removeBroker( char * buffer )
{
    int pos, found;
    for ( pos = 0, found = 0; pos < numBrokers; pos++ )
    {
        if (!found )
        {
            if ( strcmp( broker[pos].name, buffer ) == 0 )
                found = 1;
        }
        else
        {
            broker[pos-1] = broker[pos];
        }
    }
    if (found)
        broker = realloc( broker, sizeof( struct Broker ) * ( --numBrokers ) );
    return found;
}

int changeBrokerPriority( char * buffer )
{
    int pos;
    char * priorityStr = strchr( buffer, ';' );

    if ( priorityStr == NULL )
    {
        logMessage( stdout, "Failed to change Broker priority due to missing ';' delimiter.\n" );
        return 0;
    }

    // Break buffer int to 2 strings. buffer now is the broker name, and priority is the new priority
    *priorityStr = '\0';
    priorityStr++;

    long priority = strtol( priorityStr, NULL, 10 );
    if ( ( errno == ERANGE && ( priority == LONG_MAX || priority == LONG_MIN ) )
            || ( errno != 0 && priority == 0 )
            || priority < 0 || priority > 100 )
    {
        logMessage( stdout, "Failed to change Broker priority due to bad priority value.\n" );
        return 0;
    }

    // Find the broker that matches the name, and set the new priority
    for ( pos = 0; pos < numBrokers; pos++ )
    {
        if ( strcmp( broker[pos].name, buffer ) == 0 )
        {
            broker[pos].priority = priority;
            sortBrokers();
            return 1;
        }
    }
    logMessage( stdout, "Failed to change Broker priority due to bad broker name.\n" );
    return 0;

}

void printBroker( struct Broker * b, int sock ) {
    char tmp[MAX_LINE];
    snprintf(tmp, MAX_LINE, "%s; %s; %s; %ld : %s\n", b->ip, b->port, b->name, b->priority, b->status );
    write( sock, tmp, strlen( tmp ) );
}

void showBrokerMap( int sock )
{
    int i;
    for ( i = 0; i < numBrokers; i++ )
        printBroker( &broker[i], sock );
}

void showCurrentBroker( int sock )
{
    if ( currentBroker < numBrokers )
        printBroker( &broker[currentBroker-1], sock );
}


int connectToBroker( struct Broker * b )
{
    int s, sock;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    logMessage( stdout, "Connecting to %s\n", broker[currentBroker].name );

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6 addresses ok
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = (AI_V4MAPPED | AI_ADDRCONFIG);
    hints.ai_protocol = 0;


    // Get IP address
    s = getaddrinfo(broker[currentBroker].ip, broker[currentBroker].port, &hints, &result);
    if ( s != 0 )
    {
        addFaultRecord( "Invalid host name", broker + currentBroker );
        logMessage( stderr, "%s is not a valid host name: %s\n", broker[currentBroker].ip, gai_strerror(s) );
        strncpy( broker[currentBroker].status, "Invalid host name", MIN_LINE);
        freeaddrinfo(result);
        return -1;
    }

    // Traverse result linked list until we find an address we can connect to.
    for( rp = result; rp != NULL; rp = rp->ai_next )
    {

        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1)
            continue;
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) != -1)
                    break;                  /* Success */

        close(sock);
    }

    if ( rp == NULL )
    {
        addFaultRecord( "Failed to connect to host", broker + currentBroker );
        logMessage( stderr, "Unable to connect to host %s\n", broker[currentBroker].ip );
        strncpy( broker[currentBroker].status, "Failed to connect", MIN_LINE);
        freeaddrinfo(result);
        return -1;
    }

    strncpy( broker[currentBroker].status, "Connected", MIN_LINE);
    return sock;
}

void addFaultRecord( const char * msg, struct Broker * b )
{
    char tmp[MIN_LINE];
    time_t currentTime = time(NULL);

    faultHistory = realloc( faultHistory, sizeof( char * ) * ( faultCount + 1) );
    snprintf( tmp, MIN_LINE, "%.24s : %s : %s\n", ctime(&currentTime), b->name, msg );
    faultHistory[faultCount] = strdup( tmp );
    faultCount++;
}

void showFaultHistory( int sock )
{
    int i;
    for ( i = 0; i < faultCount; i++ )
        write( sock, faultHistory[i], strlen( faultHistory[i] ) );
}

void connectToNextBroker( int * sock )
{
    *sock = -1;
    if ( currentBroker > 0 )
    {
        addFaultRecord( "Lost connection to host.", broker + currentBroker - 1 );
        strncpy( broker[currentBroker-1].status, "Disconnected", MIN_LINE);
    }
    while ( *sock < 0 )
    {
        if ( currentBroker < numBrokers )
        {
            *sock = connectToBroker( &broker[currentBroker] );
            currentBroker++;
        }
        else
        {
            logMessage( stdout, "Unable to connect to a broker. Terminating\n" );
            setTerminateFlag(1);
            break;
        }
    }
}
