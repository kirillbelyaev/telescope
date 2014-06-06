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

#ifndef definitions_h
#define definitions_h

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
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <pthread.h>

#define MAX_LINE 4096
#define SERVER_PORT 50000
#define STATUS_PORT 50001
#define CLI_PORT 50002
#define S_IRWU 0644
#define R_NUM 128

#define RT_STRING "\n"
#define EXIT_STRING "exit\n"
#define EXIT_STR "exit"
#define SHUTDWN_STR "shutdown"
#define TRANSACTION_STR "change transaction"
#define SHOW_TRANSACTION_STR "show transaction"
#define RESET_TRANSACTION_STR "reset transaction"
#define HELP_STR "help"

#define DEFAULT_ACCESS_PASSWORD "telescopecq\0"

//socket stuff
struct hostent *hp;
char str[INET_ADDRSTRLEN];
char ip[MAX_LINE];
char *ipptr; 
char **pptr;

//sockets
        int server_sock;
	int peer_sock;
	int status_sock;
        int cli_sock;
        
        struct sockaddr_in server_sin;
	struct sockaddr_in status_sin;
        struct sockaddr_in cli_sin;

//xml object pointers
	struct xml data;
	struct xml *data_ptr = NULL;
	
//buffers for holding incoming xml streams	
	char buf[512*MAX_LINE];
	char * buf_ptr = NULL;
	char bgp_m[512*MAX_LINE];
	char * bgp_m_ptr = NULL;

//xml parsing globals
	char element_name[256];
	char content[256];
	int xox = -1; int zoz = -1;
	int match = -1;

//search flags for IP prefix search function
	int exactflag = 0;
	int moreflag = 0;
	int lessflag = 0;

	int serverflag = 0;//server flag
	int logflag = 0; //turn log file writing on
	int dataflag = 0; //turn data file reading on
	char filename[256];//where to write xml messages
	char DATAfilename[256];//where to read xml data file from

//parsing engine globals
	//bool g_and = true;
	
        //bool g_or;
	
        //bool complex = false;
	
        int exp_gn;

struct sigaction handler;//signal handler mainly for SIGPIPE blocking
struct sigaction term_handler;//signal handler for TERM like signals
static volatile sig_atomic_t terminateFlag = 0;
static int terminateCounter = 0; //counter to keep track of all Readers at exit
pthread_mutex_t         terminateLock;

static int threadIDCounter = 0; //counter to keep track of Reader's ID
pthread_mutex_t         threadIDLock;

char expression[MAX_LINE];/* assume the cmd-line expression is that long....  */

xmlDoc *doc = NULL;
xmlNode *root_element = NULL;

	struct sockaddr_in peer_sin;
	socklen_t len;
	int rcv;
	
	int start_len;
	int start_len_next;
	int mesg_len;

	int fflag = 0;
	int eflag = 0;

	int pflag = 0;
        int hflag = 0;

	char *fvalue = NULL;
	char *data_fvalue = NULL;
	char *evalue = NULL;

	char *pvalue = NULL;
        char *hvalue = NULL;

	int port = 0;
        
        int     syslog;
        int     loglevel;
        int     logfacility;

long long int TotalMessagesReceived = 0;
long long int MatchingMessages = 0;
FILE *logfile = NULL;
const char TeleScopeLog[] = "/var/log/telescope.log";//default log file location

//XML object structure: in - incoming raw data stream; out - refined xml data object
struct xml {
char type;
char in[512*MAX_LINE];
char out[512*MAX_LINE];
           };

const xmlChar start[] = "<xml><XML_MESSAGE length=\"00002275\"";
const xmlChar start_next[] = "<XML_MESSAGE length=\"00002275\"";
const xmlChar client_intro[] = "<xml><XML_MESSAGE length=\"00000128\" version=\"0.2\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.2\" type_value=\"0\" type=\"MESSAGE\"></XML_MESSAGE>";
const xmlChar probe[] = "<XML_MESSAGE length=\"00000128\" version=\"0.2\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.2\" type_value=\"0\" type=\"MESSAGE\"></XML_MESSAGE>";
//tmp variable ?
const xmlChar v[16*MAX_LINE];

//REG_EXP
char reg_start[256];
char regex[256];

char *regex_0_64 = "(([0-9]|[1-5][0-9]|6[0-3])\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|64\\.0)$";
char *regex_64_128 = "((6[4-9]|[7-9][0-9]|1([0-1][0-9]|2[0-7]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|128\\.0)$";
char *regex_128_192 = "((1(2[8-9]|[3-8][0-9]|9[0-1]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|192\\.0)$";
char *regex_192_255 = "((1(9[2-9])|2([0-4][0-9]|5[0-4]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|255\\.0)$";

char *regex_0_128 = "(([0-9]|[1-9][0-9]|1([0-1][0-9]|2[0-7]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|128\\.0)$";
char *regex_128_255 = "((1(2[8-9]|[3-9][0-9])|2([0-4][0-9]|5[0-4]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|255\\.0)$";

char *regex_0_255 = "(([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-4]))\\.([0-9]|[1-9][0-9]|1([0-9][0-9])|2([0-4][0-9]|5[0-5]))|255\\.0)$";

//arrays for holding parsed cmd search expressions
char tuple[1024][INET_ADDRSTRLEN];
char logic[256];
char element[256][256];
char operator[256];
char value[256][256];

//parsing engine globals
int truth[256];
char plogic[256];
char expg[256][256];

//
//functions' declarations
//

/* diagnostic routines  */ 
int firstMessage(void);
int secondMessage(void);

int prepareServerSocket(void);

//termination signals handling functions to close TeleScope gracefully
static void setTerminateFlag (int sigtype);
void terminate_(int sigtype);

//initialize the parsing engine globals
int InitializeParseEngine(char *e, int ef, char *ev);

int ReInitializeParseEngine(int eflag, char *evalue);

//returns IP for the connection
int returnip (void);

//receives and parses the xml network stream
int receive_xml(char *filename, struct xml *data_ptr);

//checks whether the IP is within a specified IP range
int ip_range(char *, char *, uint32_t pn, uint32_t cn);

//cycles through xml object and gets all the elements and their  digit or string values. 
int get_element(xmlNode * a_node, char *element_name, char operator, char *value);

//cycles through xml object and gets all the attributes and their  digit or string values. 
int get_attribute(xmlNode * a_node, char *attribute_name, char operator, char *value);

//cycles through xml object and gets all the PREFIX elements values. 
int get_prefix(xmlNode * a_node, char *element_name, char operator, char *value);

//fills the expression arrays with data to drive the exp engine
int fill(char * exp);

//magic function - analyses the xml object and writes the matched object to file
int analyze (char *filename, xmlNode *root_element);

//the same as analyze but without writing to a file
//used in complex expression handling
int analyse (xmlNode *root_element);
//int analyze2 (char *filename, xmlNode *root_element, QueueWriter writer, HashEntry item, HashTable table);

int clear(char *a, char *b, size_t s); //clear the arrays via memset()

//parsing engine functions
int mytrim(char *str);
int tokenize(char *exp);
int shuffle2(int exp_gn);
int shuffle(int exp_gn);
int harvest(char *exp, xmlNode *root_element);
int clearExpGlobals(void);
int resetExp(void);
int resetMatch(void);

int createQueues(void);
int destroyQueues(void);
int writeQueues(void * item);

int setupQueueTable(void);
int removeQueueTable(void);
int writeQueueTable(void * item);

//threading stuff
pthread_t tidw;//for single thread
pthread_t tidr;//for single thread
pthread_t *tidW;//for thread pool
pthread_t *tidR;//for thread pool
pthread_attr_t attr;
pthread_t StatusThreadID;
pthread_t CLIThreadID;
pthread_t FileReaderThreadID;
long tindex;

void *Reader(void *param);
void *FileReader(void *param);
void *StatusThread(void *param);
void *CLIThread(void *param);

void launchClientsThreadPool(void);

void launchStatusThread(void);

void launchCLIThread(void);

void launchFileReaderThread(void);

//save the peer address from the incoming client connection
int getPeerAddress (const struct sockaddr *addr, char *ip);

//write the elements of interest in the DSMS format to TeleScope.dat data file
int DSMSwrite_element(char *filename, xmlNode * a_node);

int readDataFile(int fd);

void initialize_terminateLock(void);
int update_terminateCounter(void);
int get_terminateCounter(void);

int resetXmlBuff(void);
int reset_key(void);
int godaemon (char *rundir, char *pidfile, int noclose);

void initialize_threadIDLock(void);
int update_threadIDCounter(void);
int get_threadIDCounter(void);

void print_service_ports(void);

#endif
