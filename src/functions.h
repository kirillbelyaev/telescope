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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "globals.h"

extern int logging_sock;

//! @brief signal all threads to stop execution
//! @param sigtype unused
void setTerminateFlag( int sigtype );

//! @brief termination signals handling functions to close TeleScope gracefully
//! @param sigtype unused
void terminate_( int sigtype );

//! @brief Lock a mutex, and exit on failure
//! @param m mutex to be locked
void lockMutex( pthread_mutex_t * m );

//! @brief Unlock a mutex, and exit on failure
//! @param m mutex to be unlocked
void unlockMutex( pthread_mutex_t * m );

//! @brief If signal is received, set program to properly exit
int setSignals( void );

//! @brief Send port information to stdout
void print_service_ports( void );

//! @brief Remove extra white space before and after str
//! @return 0 on success
int trim( char *str );

//! @brief report duration telescope has been running
//! @param sock socket to write output to.
void showUptime( int sock );

//! @brief Send message to log file, as well as specified stream
//! @param stream Additional destination for message. Usually stderr or stdout
//! @param format printf style format string
void logMessage( FILE *stream, const char *format, ... );

#endif
