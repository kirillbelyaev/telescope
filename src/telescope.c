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

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <regex.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

/*now we include home-brewn libs*/
#include "definitions.h"
#include "hashlib.h"
#include "functions.h"

#ifdef LIBXML_TREE_ENABLED

int main (int argc, char *argv[])
{
    int c,i;//dumb variables...
//check the command options using getopt:
while ((c = getopt (argc, argv, "sh:p:f:e:ld:")) != -1)
         switch (c)
           {
	   case 's':
             serverflag = 1;
             break;
	   case 'l':
             logflag = 1;//turn logging on - else write to /dev/null
             break;
           case 'd':
             dataflag = 1;//turn data file reading on
             data_fvalue = optarg;
             break;
	   case 'h':
             hflag = 1;
             hvalue = optarg;
             break;
	   case 'p':
             pflag = 1;
             pvalue = optarg;
             break;
           case 'f':
             fflag = 1;
             fvalue = optarg;
             break;
           case 'e':
	     eflag = 1;
             evalue = optarg;
             break;
	   case '?':
	if (optopt == 'h')
               fprintf (stderr, "Option -%c requires a hostname argument.\n", optopt);
	else if (optopt == 'p')
                fprintf(stderr, "Option -%c requires a port argument:\n", optopt);
	else if (optopt == 'f')
               fprintf (stderr, "Option -%c requires a filename argument.\n", optopt);
	else if (optopt == 'e')
               fprintf (stderr, "Option -%c requires expression argument.\n", optopt);
	else if (optopt == 'd')
               fprintf (stderr, "Option -%c requires a data filename argument.\n", optopt);
	else {
               fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt); 
             return 1;
             exit(1);
	     }
             default:
             abort ();
           }

	if (argc < 5)
        {
                fprintf (stderr, "Usage: Illegal number of arguments/options on the command line. There are 2 mandatory options to specify: -f -e.\n");
                fprintf (stderr, "Usage:\n");
		fprintf (stderr, "-s - turn on server mode\n");
		fprintf (stderr, "-l - turn logging on to file\n");
		fprintf (stderr, "-d - turn xml data file reading on (specify filename)\n");
		fprintf (stderr, "-h - hostname to connect to\n");
		fprintf (stderr, "-p - port number\n");
		fprintf (stderr, "-f - filename to write the captured xml messages\n");
		fprintf (stderr, "-e - query expression to match in the xml stream\n");
                return -1;
        }

//checking invalid arguments...
for (i = optind; i < argc; i++)
{
printf ("Non-option argument %s\n", argv[i]);
	return -1;
}

if (fflag != 1){
    perror("file to capture messages not defined! please specify the filename via the -f flag. Indicate /dev/null if not interested in capturing messages on disk.");
    exit(-1);
               }         
         
             if (strlen(fvalue) > 252 )
		{
		perror("FATAL: filename argument length is too long! Avoiding buffer overrun!");
		exit(-1);
		}
		
		if (dataflag == 1)
				{
             if (strlen(data_fvalue) > 252 )
		{
		perror("FATAL: data filename argument length is too long! Avoiding buffer overrun!");
		exit(-1);
		}
				}

if (eflag != 1){
    perror("expression not defined! please specify query via the -e flag");
    exit(-1);
               }
         
//client mode
if (hflag == 1 && pflag == 1 && serverflag == 0) {
port = atoi (pvalue);
if (port == 0){
perror("port not defined!");
exit(-1);
	      } 
					    	 }

//dual-mode: client-server
if (hflag == 1 && pflag == 1 && serverflag == 1) {
port = atoi (pvalue);
if (port == 0){
perror("port not defined!");
exit(-1);
	      } 
					    	 }
//if hostname is specified but port is not
if (hflag == 1 && pflag == 0) {
perror("port not defined!");
exit(-1);
			      }
	
	strcpy(filename, fvalue);
	if (dataflag == 1)
	strcpy(DATAfilename, data_fvalue);

	sprintf(element_name, fvalue);

	//open debug log file
	if (logflag == 1)
	{
	if ((logfile = fopen(TeleScopeLog, "w+")) == NULL) {
		perror("Unable to write to the /var/log/telescope.log logfile! Probably lacking permission. Logging to /dev/null instead.");
		logfile = fopen("/dev/null", "w+");
							   } else
        fprintf(logfile, "Log started for this session of TeleScope. Writing to /var/log/telescope.log\n");
	} else {
	logfile = fopen("/dev/null", "w+");
        fprintf(stdout, "Logging is not turned on - writing to /dev/null for this session of TeleScope:\n");
	       }

if (port != 0)
{    
        hp = gethostbyname(hvalue);
        ipptr = ip;  

        if (!hp) {
                fprintf (stderr, "unknown host: %s\n", hvalue);
                exit(1);
                 } else returnip();
}


print_service_ports();    /* Mon May 26 17:15:55 MDT 2014 */    

        
if (serverflag == 1)
{
    fprintf(stdout, "Becoming a daemon since server mode of operation is specified. Bye!\n");
    daemon(0, 0);//using C library facility to simplify daemon things for now...
}


InitializeParseEngine(Expression, eflag, evalue);
setSignals();

if (hflag == 1 && pflag == 1) //if operating in the subscriber mode
        establishPeerConnection();

//if operating in server mode - do server stuff...
if (serverflag == 1) {
setupQueueTable();
initialize_threadIDLock();
    
prepareStatusThread();
launchStatusThread();

prepareCLIThread();
launchCLIThread();

if (dataflag == 1)
        launchFileReaderThread();

prepareServerSocket();    
launchClientsThreadPool();
                     }//server mode case end

/*reading XML data starts here - we do it in main for first 2-3 messages for
diagnostic purposes - if input is valid then we proceed further, if not - it is easy to spot the place we choke
we read initial xml_message first */

if (hflag == 1 && pflag == 1) //if operating in the subscriber mode
{    
        firstMessage();
        //now we read all subsequent xml_messages
        secondMessage();
    
        /* the main loop starts    */
        processStream_static_buffer();
        //Never reached unless interrupted by a TERM signal....
        terminate_(0);
}

while (terminateFlag != 1); //if operating in the publisher mode

//Never reached unless interrupted by a TERM signal....
terminate_(0);

return 0;
}//end of main()

#else
int main(void) 
{
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif