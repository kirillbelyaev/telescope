/*
* Copyright (c) 2014-2018  Kirill Belyaev
* kirillbelyaev@yahoo.com
* kirill@cs.colostate.edu
* TeleScope - XML Message Stream Broker/Replicator Platform
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send
* a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*
* Author: Vignesh M. Pagadala
* Vignesh.Pagadala@colostate.edu
* Last update: December 2017
*
* This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit
* (http://www.openssl.org/).
*
* File: Control_Message_Processor.h
* ----------------------------------
* Functions for parsing an XML control message, filtering out the Upstream Brokers List and Filtering 
* Query from the message and performing validation using Secure Hash Algorithm 1.
*/

#ifndef _CONTROL_MESSAGE_PROCESSOR_H
#define _CONTROL_MESSAGE_PROCESSOR_H

#include "Control_Message_Processor_Globals.h"

/*
* @desc Calls filter(). For performing further processing.
* @param The XML control message to be processed.
* @return -1 if the message is invalid. 0 if valid..
*/
int process(char *buff);

/*
* @desc Parses XML message and validates it.
* @param The XML control message to be processed.
* @return -1 if the message is invalid. 0 if valid.
*/
int_fast8_t filter(char *buff);

/*
* @desc Returns Upstream Brokers List of most recently process()ed control message.
* @usage After a call to process() has been made; before a call to freeMem().
* @return String with Upstream Brokers List.
*/
char* get_UBL(void);

/*
* @desc Returns Filtering Query of most recently process()ed control message.
* @usage After a call to process(); before a call to freeMem().
* @return String with Filtering Query.
*/
char* get_FQ(void);

/*
* @desc Computes SHA-1 digest of control message.
* @param The XML control message who's SHA-1 is to be computed.
* @return String with SHA-1 digest of the XML message (with empty SHA-1 element).
*/
char* shafunc(char *buff);

/*
* @desc Deallocates memory.
* @usage After a call to process() and after optional calls to get_UBL() and get_FQ().
*/
void freeMem(void);

/*
* @desc Reads UBL of most recently process()ed control message and prints it out.
* @usage After a call to process(); before a call to freeMem().
*/
void printUBL();

/*
* @desc Introduces a time-delay.
* @param Number of seconds to delay.
*/
void delay(unsigned int seconds);

#endif
