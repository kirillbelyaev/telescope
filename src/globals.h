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

#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <fcntl.h>
#include <regex.h>
#include <signal.h>
#include <math.h>
#include <sys/stat.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <pthread.h>

// Local library functions
#include "queuelib.h"
#include "functions.h"
#include "cli.h"
#include "status.h"
#include "parserEngine.h"
#include "fileReader.h"
#include "connectionManager.h"
#include "passwordManager.h"
#include "server.h"
#include "subscriber.h"

#define MIN_LINE 256
#define MAX_LINE 4096
#define DEFAULT_STARTING_PORT 50000
#define SERVER_PORT 0
#define STATUS_PORT 1
#define CLI_PORT 2
#define S_IRWU 0644

#define RT_STRING "\n"
#define EXIT_STRING "exit"
#define SHUTDWN_STR "shutdown"
#define TRANSACTION_STR "change transaction"
#define SHOW_TRANSACTION_STR "show transaction"
#define RESET_TRANSACTION_STR "reset transaction"
#define HELP_STR "help"

#define DEFAULT_ACCESS_PASSWORD "telescopecq"

extern volatile sig_atomic_t terminateFlag;

extern int serverflag;
extern long startingPort;
extern time_t serverStartTime;

FILE * logfile;

#endif
