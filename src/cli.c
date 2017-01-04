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

#include "cli.h"

int cli_sock;
pthread_t CLIThreadID;

int prepareCLIThread( void );
void *CLIThread( void *thread_info );

const char * prompt = "$_: ";
const char * newTransactionPrompt = "Enter new transaction: ";
const char * addBrokerPrompt = "Enter new broker in the form \"ip; port; name; priority\"\n"
                               "(e.g. \"127.0.0.1; 50000; testServer; 1\"): ";
const char * removeBrokerPrompt = "Enter the name of the broker to remove: ";
const char * changeBrokerPriorityPrompt = "Enter name of the broker, and it's new priority in the format \"name; priority\"\n"
                                          "(e.g. \"testServer; 100\"): ";
const char * errorMessage = "unknown command. type h for help\n";
const char * successMessage = "Success\n";
const char * failMessage = "Failed\n";
const char * helpMessage = "available commands are :\n"
                           "  help (h)\n"
                           "  exit (q)\n"
                           "  show transaction (st)\n"
                           "  change transaction (ct)\n"
                           "  reset transaction (rt)\n"
                           "  add broker (ab)\n"
                           "  remove broker (rb)\n"
                           "  change broker priority (cbp)\n"
                           "  show map (sm)\n"
                           "  show current broker (scb)\n"
                           "  show fault history (sfh)\n"
                           "  show uptime (su)\n"
                           "  change password (cp)\n"
                           "  shutdown (sd)\n";

void launchCLIThread( )
{
    cli_sock = prepareCLIThread();
    //Launch the Status Thread
    /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */
    if ( pthread_create( &CLIThreadID, NULL, CLIThread, (void *)&cli_sock )
            != 0 )
    {
        perror( "failed to create a CLI Thread!" );
        exit( -1 );
    }
}

int prepareCLIThread( void )
{
    struct sockaddr_in cli_sin;
    int x = 1, cli_sock;
    //build status address data structure
    cli_sin.sin_family = AF_INET;
    cli_sin.sin_addr.s_addr = INADDR_ANY;
    cli_sin.sin_port = htons( startingPort + CLI_PORT );

    // memset(&cli_sin.sin_zero, '\0', sizeof(&cli_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

    /* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
    memset( &cli_sin.sin_zero, '\0', sizeof( *cli_sin.sin_zero ) ); /* Wed May 21 11:33:42 PDT 2014 */

    if ( ( cli_sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror( "can't open a CLI socket!" );
        exit( 1 );
    }
    //lets reuse the socket!
    setsockopt( cli_sock, SOL_SOCKET, SO_REUSEADDR, &x, sizeof( x ) );

    if ( ( bind( cli_sock, (struct sockaddr *)&cli_sin, sizeof( cli_sin ) ) )
            < 0 )
    {
        perror( "can't bind to a CLI socket!" );
        exit( 1 );
    }

    //lets put the Status socket into listening mode to accept Status requests
    if ( listen( cli_sock, 1 ) < 0 )
    {
        perror( "can't start listening on a CLI socket!" );
        exit( 1 );
    }
    return cli_sock;
}

int receivedInBuffer( char * buffer, int sock, int readSize )
{
    int result;
    memset( buffer, '\0', readSize );
    if ( ( result = recv( sock, buffer, readSize-1, 0 ) ) > 0 )
        trim(buffer);
    return result;
}

void *CLIThread( void *thread_info )
{
    int sock;
    int lsock = *( (int *)thread_info);
    char buffer[MAX_LINE];

    logMessage( stdout, "CLI Thread started. Listening on port %ld \n", startingPort + CLI_PORT );

    // detach the thread so the resources may be returned when the thread exits
    pthread_detach( pthread_self() );

    memset( buffer, '\0', sizeof(buffer) );

    while ( terminateFlag == 0 )
    {    //outer loop starts
        if ( ( sock = accept( lsock, NULL, NULL ) ) < 0 )
        {
            perror( "CLI Thread: can't accept on a socket!" );
            close( sock );
            continue;
        }
        else
        {
            // Setup log messages to be sent through the CLI socket;
            logging_sock = sock;

            memset( buffer, '\0', sizeof(buffer) );

            if ( authenticateUser( sock ) )
            {
                for ( ; terminateFlag == 0 ; )
                {

                    memset( buffer, '\0', sizeof(buffer) );

                    write( sock, prompt, strlen( prompt ) );
                    if ( receivedInBuffer( buffer, sock, MAX_LINE ) <= 0 )
                        break;
                    if ( strcmp( buffer, EXIT_STRING ) == 0
                            || strcmp( buffer, "q" ) == 0 )
                        break;
                    else if ( strcmp( buffer, HELP_STR ) == 0
                            || strcmp( buffer, "h" ) == 0 )
                        write( sock, helpMessage, strlen( helpMessage ) );
                    else if ( strcmp( buffer, SHUTDWN_STR ) == 0
                            || strcmp( buffer, "sd" ) == 0 )
                        setTerminateFlag(1);
                    else if ( strcmp( buffer, RESET_TRANSACTION_STR ) == 0
                            || strcmp( buffer, "rt" ) == 0 )
                        InitializeParseEngine( "" );
                    else if ( strcmp( buffer, SHOW_TRANSACTION_STR ) == 0
                            || strcmp( buffer, "st" ) == 0 )
                    {
                        write( sock, getExpression(),
                            strlen( getExpression() ) );
                        write( sock, "\n", 1 );
                    }
                    else if ( strcmp( buffer, TRANSACTION_STR ) == 0
                            || strcmp( buffer, "ct" ) == 0 )
                    {
                        write( sock, newTransactionPrompt, strlen( newTransactionPrompt ) );
                        if ( receivedInBuffer( buffer, sock, MAX_LINE ) >= 2 )
                        {
                            if ( strlen( buffer ) <= 4 )
                                continue; //we assume the expression smaller then 4 characters is a fake one
                            else InitializeParseEngine( buffer );
                        }
                    }
                    else if ( strcmp( buffer, "ab" ) == 0 )
                    {
                        write( sock, addBrokerPrompt, strlen( addBrokerPrompt ) );
                        if ( receivedInBuffer( buffer, sock, MAX_LINE ) > 0 )
                        {
                            if ( addBroker( buffer ) )
                                write( sock, successMessage, strlen( successMessage ) );
                            else
                                write( sock, failMessage, strlen( failMessage ) );
                        }

                    }
                    else if ( strcmp( buffer, "rb" ) == 0 )
                    {
                        write( sock, removeBrokerPrompt, strlen( removeBrokerPrompt ) );
                        if ( receivedInBuffer( buffer, sock, MAX_LINE ) > 0 )
                        {
                            if ( removeBroker( buffer ) )
                                write( sock, successMessage, strlen( successMessage ) );
                            else
                                write( sock, failMessage, strlen( failMessage ) );
                        }
                    }
                    else if ( strcmp( buffer, "cbp" ) == 0 )
                    {
                        write( sock, changeBrokerPriorityPrompt, strlen( changeBrokerPriorityPrompt ) );
                        if ( receivedInBuffer(buffer, sock, MAX_LINE ) )
                        {
                            if ( changeBrokerPriority( buffer ) )
                                write( sock, successMessage, strlen( successMessage ) );
                            else
                                write( sock, failMessage, strlen( failMessage ) );
                        }
                    }
                    else if ( strcmp( buffer, "sm" ) == 0 )
                        showBrokerMap( sock );
                    else if ( strcmp( buffer, "scb" ) == 0 )
                        showCurrentBroker( sock );
                    else if ( strcmp( buffer, "sfh" ) == 0 )
                        showFaultHistory( sock );
                    else if ( strcmp( buffer, "su" ) == 0 )
                        showUptime( sock );
                    else if ( strcmp( buffer, "cp" ) == 0 )
                        setNewPassword( sock );
                    else if ( buffer[0] == '\0' )
                        ; // Do nothing except write the prompt again on empty string
                    else
                        write( sock, errorMessage, strlen( errorMessage ) );
                } //end of for loop

                logging_sock = 0;
                close( sock ); //terminate connection immediately upon exit
            }
            else
            {
                logging_sock = 0;
                close( sock ); //end of passwd checking - if passwd does not match
            }
            //printf("buffer len is: %d ", strlen(buffer));
        } //end of else for accept()
    } //end of outer while loop
    pthread_exit( (void *)1 );
} //end of CLI Thread
