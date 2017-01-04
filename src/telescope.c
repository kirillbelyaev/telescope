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

#include "globals.h"

#ifdef LIBXML_TREE_ENABLED

long startingPort = DEFAULT_STARTING_PORT;
int serverflag = 0;
time_t serverStartTime;

int main( int argc, char *argv[] ) {
    int c, i; //dumb variables...
    int logflag = 0; //turn log file writing on
    int subFlag = 0;

    char *fvalue = NULL;
    char *data_fvalue = NULL;
    char *evalue = NULL;
    char *ovalue = NULL;

    char *pvalue = NULL;
    const char TeleScopeLog[] = "/var/log/telescope.log"; //default log file location

    char * hvalue = NULL;

    //check the command options using getopt:
    while ( ( c = getopt( argc, argv, "sh:o:p:f:e:lud:c:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'c':
                pwFile = optarg;
                break;
            case 'd':
                data_fvalue = optarg;
                break;
            case 'e':
                evalue = optarg;
                break;
            case 'f':
                fvalue = optarg;
                break;
            case 'h':
                subFlag = 1;
                hvalue = optarg;
                break;
            case 'l':
                logflag = 1; //turn logging on - else write to /dev/null
                break;
            case 'o':
                ovalue = optarg;
                break;
            case 'p':
                subFlag = 1;
                pvalue = optarg;
                break;
            case 's':
                serverflag = 1;
                break;
            case 'u':
                subFlag = 1;
                break;
            case '?':
                if ( optopt == 'h' ) fprintf( stderr,
                    "Option -%c requires a hostname argument.\n", optopt );
                else if ( optopt == 'p' ) fprintf( stderr,
                    "Option -%c requires a port argument:\n", optopt );
                else if ( optopt == 'f' ) fprintf( stderr,
                    "Option -%c requires a filename argument.\n", optopt );
                else if ( optopt == 'e' ) fprintf( stderr,
                    "Option -%c requires expression argument.\n", optopt );
                else if ( optopt == 'd' ) fprintf( stderr,
                    "Option -%c requires a data filename argument.\n", optopt );
                else {
                    fprintf( stderr, "Unknown option character `\\x%x'.\n",
                        optopt );
                }
                return 1;
            default:
                return 1;
        }
    }

    // Print usage if no arguments provided
    if ( argc < 2 )
    {
        fprintf( stderr, "Usage:\n" );
        fprintf( stderr, "  Subscriber Mode: %s (-u|-h host -p port) -f (datafile) -e (expression) )\n", argv[0] );
        fprintf( stderr, "    -u - turn on subscriber mode (subscribe to broker from topology.cnf)\n" );
        fprintf( stderr, "    -h - initial host to connect to in subscriber mode\n" );
        fprintf( stderr, "    -p - initial port number in subscriber mode\n" );
        fprintf( stderr, "    -f - filename to write the captured xml messages\n" );
        fprintf( stderr, "    -e - query expression to match in the xml stream\n" );
        fprintf( stderr, "  Server Mode: %s -s [-d (datafile)] [-o port] [-c pwfile]\n", argv[0] );
        fprintf( stderr, "    -s - turn on server mode\n" );
        fprintf( stderr, "    -l - turn logging on to file\n" );
        fprintf( stderr, "    -d - turn xml data file reading on (specify filename)\n" );
        fprintf( stderr, "    -o - outgoing connection port number\n" );
        fprintf( stderr, "    -c - password file\n" );
        fprintf( stderr, "  Server/Subscriber Mode: %s (subscriber mode arguments) (server mode arguments)\n", argv[0] );
        return -1;
    }

    //checking invalid arguments...
    for ( i = optind; i < argc; i++ )
    {
        printf( "Non-option argument %s\n", argv[i] );
        return -1;
    }

    //subscriber mode checks
    if ( subFlag )
    {
        if ( !fvalue )
        {
            perror( "File to capture messages not defined while in subscriber "
                    "mode! Please specify the filename via the -f flag. Indicate "
                    "/dev/null if not interested in capturing messages on disk." );
            exit( -1 );
        }
        if ( strlen( fvalue ) > 255 )
        {
            perror( "Filename argument length is too long! Maximum Linux file "
                    "name length is 255 characters!" );
            exit( -1 );
        }

        if (hvalue && !pvalue)
        {
            perror( "host defined but port not defined.");
        }

        if ( !evalue )
        {
            perror( "expression not defined! please specify query via the -e "
                    "flag." );
            exit( -1 );
        }
    }
    else
    {
        if ( evalue )
            perror( "-e option has no effect when not in subscriber mode." );
        if ( fvalue )
            perror( "-f option has no effect when not in subscriber mode." );

    }

    //server mode checks
    if ( serverflag )
    {
        serverStartTime = time(NULL);
        if ( data_fvalue )
        {
            if ( strlen( data_fvalue ) > 255 ) {
                perror( "Data filename argument length is too long! Maximum Linux file "
                    "name length is 255 characters!" );
                exit( -1 );
            }
        }
        if ( ovalue )
        {
            startingPort = strtol( ovalue, NULL, 10 );
            if ( ( errno == ERANGE && ( startingPort == LONG_MAX || startingPort == LONG_MIN ) )
                    || ( errno != 0 && startingPort == 0 ) )
            {
                perror( "Invalid Server port number" );
                return -1;
            }
        }
    }
    else
    {
        if ( data_fvalue )
            perror( "-d option has no effect when not in server mode." );
        if ( ovalue )
            perror( "-o option has no effect when not in server mode." );
    }



    // Test libxml2 version is compatible
    LIBXML_TEST_VERSION


    //open debug log file
    if ( logflag == 1 ) {
        if ( ( logfile = fopen( TeleScopeLog, "w+" ) ) == NULL ) {
            perror(
                "Unable to write to the /var/log/telescope.log logfile! Probably lacking permission. Logging to /dev/null instead." );
            logfile = fopen( "/dev/null", "w+" );
        } else fprintf( logfile,
            "Log started for this session of TeleScope. Writing to /var/log/telescope.log\n" );
    } else {
        logfile = fopen( "/dev/null", "w+" );
        fprintf( stdout,
            "Logging is not turned on - writing to /dev/null for this session of TeleScope:\n" );
    }

    setSignals();

    //server mode...
    if ( serverflag == 1 )
    {
        print_service_ports();
        fprintf( stdout, "Becoming a daemon since server mode of operation is "
                         "specified. Bye!\n" );
        //daemon( 0, 0 ); //using C library facility to simplify daemon things for now...


        launchStatusThread();
        launchCLIThread( );
        launchServerThreadPool();
        if ( data_fvalue ) launchFileReaderThread( data_fvalue );
    }

    //subscriber mode
    if ( subFlag )
    {

        InitializeParseEngine( evalue );
        launchSubscriberThread( hvalue, pvalue, fvalue );
    }

    // keep main thread watching terminate flag since the subscriber thread can
    // block in recv function, and sd command from cli will not shut down telescope
    while ( terminateFlag != 1 )
        // Wait 1ms to prevent 100% CPU utilization.
        usleep(1000);

    //Never reached until server is shutdown or interrupted by a TERM signal....
    terminate_( 0 );

    return 0;
} //end of main()

#else

int main(void)
{
    fprintf(stderr, "Tree support not compiled in\n");
    return 1;
}

#endif
