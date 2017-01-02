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

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "globals.h"

#define TOPOLOGY_CNF "topology.cnf"

//! Initialize the broker list.

//! Sets up the first broker from the command line with priority of 100, and
//! reads topology.cnf if it exists to the rest of the broker list.
//! @param host command line host name
//! @param port command line port number
void setupBrokerList( const char * host, const char * port );

//! @brief free memory allocated by the broker list
void deleteConnectionManager();

//! @brief Add new broker to the list
//! @param buffer string in the form "ip; port; name; priority"
//! @return 1 on success 0 on fail
int addBroker( char * buffer );

//! @brief Remove new broker from the list
//! @param buffer name of the broker to remove
//! @return 1 on success 0 on fail
int removeBroker( char * buffer );

//! @brief Remove new broker from the list
//! @param buffer string in the form "name; priority"
//! @return 1 on success 0 on fail
int changeBrokerPriority( char * buffer );

//! @brief print the broker map to a socket
//! @arg sock - socket file descriptor
void showBrokerMap( int sock );

//! @brief print the current broker map to a socket
//! @arg sock - socket file descriptor
void showCurrentBroker( int sock );

//! @brief print information on past broker failures to a socket
//! @arg sock - socket file descriptor
void showFaultHistory( int sock );

//! @brief Updates value of sock with a connection to the next broker on the broker list.
//! @param sock address of socket file descriptor to update.
void connectToNextBroker( int * sock );

#endif
