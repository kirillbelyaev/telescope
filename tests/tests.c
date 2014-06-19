/*
 * File:   newsimpletest.c
 * Author: kirill
 *
 * way to compile:
 * gcc -g -DHAVE_CONFIG_H -I/usr/include/libxml2 -lxml2 -lm -lz  tests.c -o tests.o
 * 
 * Created on Feb 17, 2013, 2:21:45 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "definitions.h"

#define MAX_LINE 4096

char expg[256][256];

int exp_gn;
char expression[MAX_LINE];

const xmlChar valid_message[] = "<BGP_MESSAGE length=\"00002208\" version=\"0.4\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.4\" type_value=\"2\" type=\"UPDATE\"><BGPMON_SEQ id=\"2128112124\" seq_num=\"2017269815\"/><TIME timestamp=\"1402361117\" datetime=\"2014-06-10T00:45:17Z\" precision_time=\"732\"/><PEERING as_num_len=\"2\"><SRC_ADDR><ADDRESS>192.43.217.144</ADDRESS><AFI value=\"1\">IPV4</AFI></SRC_ADDR><SRC_PORT>179</SRC_PORT><SRC_AS>14041</SRC_AS><DST_ADDR><ADDRESS>129.82.138.6</ADDRESS><AFI value=\"1\">IPV4</AFI></DST_ADDR><DST_PORT>4321</DST_PORT><DST_AS>6447</DST_AS><BGPID>0.0.0.0</BGPID></PEERING><ASCII_MSG length=\"88\"><MARKER length=\"16\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF</MARKER><UPDATE withdrawn_len=\"0\" path_attr_len=\"61\"><WITHDRAWN count=\"0\"/><PATH_ATTRIBUTES count=\"4\"><ATTRIBUTE length=\"1\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"1\">ORIGIN</TYPE><ORIGIN value=\"0\">IGP</ORIGIN></ATTRIBUTE><ATTRIBUTE length=\"12\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"2\">AS_PATH</TYPE><AS_PATH><AS_SEG type=\"AS_SEQUENCE\" length=\"5\"><AS>14041</AS><AS>11164</AS><AS>7473</AS><AS>17888</AS><AS>10098</AS></AS_SEG></AS_PATH></ATTRIBUTE><ATTRIBUTE length=\"4\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"3\">NEXT_HOP</TYPE><NEXT_HOP>192.43.217.144</NEXT_HOP></ATTRIBUTE><ATTRIBUTE length=\"32\"><FLAGS optional=\"TRUE\" transitive=\"TRUE\"/><TYPE value=\"8\">COMMUNITIES</TYPE><COMMUNITIES><COMMUNITY><AS>7473</AS><VALUE>10000</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>20000</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>31203</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>31302</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>41204</VALUE></COMMUNITY><COMMUNITY><AS>11164</AS><VALUE>1120</VALUE></COMMUNITY><COMMUNITY><AS>11164</AS><VALUE>7880</VALUE></COMMUNITY><COMMUNITY><AS>14041</AS><VALUE>202</VALUE></COMMUNITY></COMMUNITIES></ATTRIBUTE></PATH_ATTRIBUTES><NLRI count=\"1\"><PREFIX label=\"SPATH\"><ADDRESS>202.123.88.0/24</ADDRESS><AFI value=\"1\">IPV4</AFI><SAFI value=\"1\">UNICAST</SAFI></PREFIX></NLRI></UPDATE></ASCII_MSG><OCTET_MSG><OCTETS length=\"88\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0058020000003D4001010040020C020536D92B9C1D3145E02772400304C02BD990C008201D3127101D314E201D3179E31D317A461D31A0F42B9C04602B9C1EC836D900CA18CA7B58</OCTETS></OCTET_MSG></BGP_MESSAGE>";

const xmlChar invalid_message[] = "<BGP_MESSAGE length=\"00002208\" version=\"0.4\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.4\" type_value=\"2\" type=\"UPDATE\"><BGPMON_SEQ id=\"2128112124\" seq_num=\"2017269815\"/><TIME timestamp=\"1402361117\" datetime=\"2014-06-10T00:45:17Z\" precision_time=\"732\"/><PEERING as_num_len=\"2\"><SRC_ADDR><ADDRESS>192.43.217.144</ADDRESS><AFI value=\"1\">IPV4</AFI></SRC_ADDR><SRC_PORT>179</SRC_PORT><SRC_AS>14041</SRC_AS><DST_ADDR><ADDRESS>129.82.138.6</ADDRESS><AFI value=\"1\">IPV4</AFI></DST_ADDR><DST_PORT>4321</DST_PORT><DST_AS>6447</DST_AS><BGPID>0.0.0.0</BGPID></PEERING><ASCII_MSG length=\"88\"><MARKER length=\"16\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF</MARKER><UPDATE withdrawn_len=\"0\" path_attr_len=\"61\"><WITHDRAWN count=\"0\"/><PATH_ATTRIBUTES count=\"4\"><ATTRIBUTE length=\"1\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"1\">ORIGIN</TYPE><ORIGIN value=\"0\">IGP</ORIGIN></ATTRIBUTE><ATTRIBUTE length=\"12\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"2\">AS_PATH</TYPE><AS_PATH><AS_SEG type=\"AS_SEQUENCE\" length=\"5\"><AS>14041</AS><AS>11164</AS><AS>7473</AS><AS>17888</AS><AS>10098</AS></AS_SEG></AS_PATH></ATTRIBUTE><ATTRIBUTE length=\"4\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"3\">NEXT_HOP</TYPE><NEXT_HOP>192.43.217.144</NEXT_HOP></ATTRIBUTE><ATTRIBUTE length=\"32\"><FLAGS optional=\"TRUE\" transitive=\"TRUE\"/><TYPE value=\"8\">COMMUNITIES</TYPE><COMMUNITIES><COMMUNITY><AS>7473</AS><VALUE>10000</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>20000</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>31203</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>31302</VALUE></COMMUNITY><COMMUNITY><AS>7473</AS><VALUE>41204</VALUE></COMMUNITY><COMMUNITY><AS>11164</AS><VALUE>1120</VALUE></COMMUNITY><COMMUNITY><AS>11164</AS><VALUE>7880</VALUE></COMMUNITY><COMMUNITY><AS>14041</AS><VALUE>202</VALUE></COMMUNITY></COMMUNITIES></ATTRIBUTE></PATH_ATTRIBUTES><NLRI count=\"1\"><PREFIX label=\"SPATH\"><ADDRESS>202.123.88.0/24</ADDRESS><AFI value=\"1\">IPV4</AFI><SAFI value=\"1\">UNICAST</SAFI></PREFIX></NLRI></UPDATE></ASCII_MSG><OCTET_MSG><OCTETS length=\"88\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0058020000003D4001010040020C020536D92B9C1D3145E02772400304C02BD990C008201D3127101D314E201D3179E31D317A461D31A0F42B9C04602B9C1EC836D900CA18CA7B58</OCTETS></OCTET_MSG></BGP_MESSAGE...";


/*
 * Simple C Test Suite
 */

void test1() {
    printf("newsimpletest test 1\n");
}

void test2() {
    printf("newsimpletest test 2\n");
    printf("%%TEST_FAILED%% time=0 testname=test2 (newsimpletest) message=error message sample\n");
}

int tokenize(char *exp)
{
int i, h, l, k;
i=0; h=0; l=0;
k= strlen(exp);
while (l < k)
{
	if (exp[l] != '(' && exp[l] != ')') {
 	expg[h][i] = exp[l];	
		i++;
		l++;
			   		    }		
	else {
		i=0;
		if (l != 0)
		h++;

		l++;
	     }
}
	return h;
}


void testLoop(void)
{
    int i,j;
    int start_len_next;
    //start_len_next = strlen("hello");
    start_len_next = strlen("hello world 1 2 3 4 5 6 7 8 !");
    
    
    printf("%d, %d \n", i, j);
    printf("string len is: %d \n", start_len_next);
    
    for (i=21, j=0; i<start_len_next, j < 8; i++, j++)
        printf("%d, %d \n", i, j);
    
    printf("modified for loop: \n");
    
    for (i=21, j=0; j < 8; i++, j++)
        printf("%d, %d \n", i, j);
    
                                                 
}


void testSwap(void)
{
    long swap = 0;
    long tail = 10;
    
    swap = tail--;
    printf("tail is: %d \n", tail);
    printf("swap is: %d \n", swap);
    
    tail = swap;
    
    printf("swap & tail are: %d, %d \n", swap, tail);
    
    
}

void testSwap2(void)
{
    long swap = 0;
    long tail = 10;
    
    tail--;
    printf("tail is: %d \n", tail);
    
    swap = tail;
    tail = swap;
    
    printf("swap & tail are: %d, %d \n", swap, tail);
    
    
}


void testDecrement(void)
{
    long tail = 10;
    
    tail--;
    
    printf("tail is: %d \n", tail);
    
    tail = tail--;
    
    printf("tail is: %d \n", tail);
    
    tail = tail % 100;
    
    printf("tail is: %d \n", tail);
    
    
}

int receive_xml(char *filename, struct xml *data_ptr)
{
   // return -1;
    return 0;
}


void processStream(void)
{
 while (terminateFlag != 1) {

data_ptr = malloc(sizeof(data));

if (data_ptr == NULL) { data_ptr = NULL; return; } //KB: have to check memory allocation

strcpy(data_ptr->in, "");
strcpy(data_ptr->out, "");


printf("processStream: about to call receive_xml() \n");

if (receive_xml(filename, data_ptr) == -1) {


memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));

data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL; //KB: set to NULL -  without setting to NULL telescope was giving seg. faults when stream is interrupted 
// break; /*  Sun May 25 15:53:50 PDT 2014 */
continue; /* continue operating if the message is malformed just skip it. Sun May 25 15:53:50 PDT 2014 */

                                           }

printf("processStream: reached only if receive_xml() returns 0 \n");

memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));
data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL;

          		   }//end of main loop   
}

void processStreamMock(void)
{
 while (terminateFlag != 1) {

data_ptr = NULL;

if (data_ptr == NULL) { data_ptr = NULL; return; } //KB: have to check memory allocation

printf("processStream: about to call receive_xml() \n");

if (receive_xml(filename, data_ptr) == -1) {

data_ptr = NULL; //KB: set to NULL -  without setting to NULL telescope was giving seg. faults when stream is interrupted 
// break; /*  Sun May 25 15:53:50 PDT 2014 */
continue; /* continue operating if the message is malformed just skip it. Sun May 25 15:53:50 PDT 2014 */

                                           }

printf("processStream: reached only if receive_xml() returns 0 \n");

data_ptr = NULL;

          		   }//end of main loop   
}


int receive_xml_2(char *buffer, struct xml *data_ptr)
{
xmlDoc *doc = NULL;
xmlNode *root_element = NULL;
int i,j;
char filter[8];
char refilter[9];

int start_len_next;
int msg_len;

//rcv = recv(peer_sock, data_ptr->in, xmlStrlen(start_next), MSG_WAITALL);
//strcpy(data_ptr->out, data_ptr->in);

//start_len_next = strlen(data_ptr->in);
//start_len_next = xmlStrlen(start_next);

//extract the real length
//for (i=21, j=0; i<start_len_next, j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

strcpy(data_ptr->in, buffer);


/* avoid left-hand operand of comma expression has no effect [-Wunused-value] in gcc 4.8+ */
for (i=21, j=0; j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

        filter[j] = data_ptr->in[i];
                                                   }
strncpy(refilter, filter, 7);
refilter[7] = filter[7];
refilter[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

msg_len = atoi(refilter);

if (msg_len >= MAX_LINE*MAX_LINE) /* skip message larger then a predefined size:  Tue Jun 10 11:43:47 PDT 2014 */
{
    fprintf(stderr, "receive_xml(): Message is too big! Skipping.\n");
    return -1;
}

strcpy(data_ptr->out, data_ptr->in);

/*
printf("msg_len is: %d\n", msg_len);
printf("data_ptr->out is: %s\n", data_ptr->out);

printf("in data_ptr->out character is: %c\n", data_ptr->out[0]);
printf("in data_ptr->out character is: %c\n", data_ptr->out[msg_len-1]);
printf("in data_ptr->out character is: %c\n", data_ptr->out[msg_len]);
*/


/*/
if (data_ptr->out[msg_len-1] != '>')
{
        fprintf(stderr, "receive_xml(): invalid xml document in the buffer! skipping.\n");
        return (-1);
}
*/

//strcpy(data_ptr->in, "");

//strcpy(data_ptr->in, "\0"); /* Sat Jun 14 10:13:59 PDT 2014 */

//rcv = recv(peer_sock, data_ptr->in, msg_len - start_len_next, MSG_WAITALL);

//strncat(data_ptr->out, data_ptr->in, msg_len - start_len_next); //stronger checking condition in case recv() is fooled...
//fprintf(logfile, "now we parse the data_ptr->out_m in memory and get all the elements\n" );

/* introduce actual naive xml validity check before calling xmlParseMemory() to avoid seg faults caused by the library:  Sat Jun 14 10:13:59 PDT 2014 */
if (data_ptr->out[0] != '<' || data_ptr->out[msg_len-1] != '>' || data_ptr->out[msg_len] != '\0')
{
        fprintf(stderr, "receive_xml(): invalid xml document in the buffer! skipping.\n");
        return (-1);
}

//exit(0);

//XML stuff

//doc = xmlReadMemory(data_ptr->out, sizeof(data_ptr->out), "next_bgp_m.xml", NULL, 0);
//doc = xmlReadMemory(data_ptr->out, strlen(data_ptr->out), "buffer.xml", NULL, 0);

//doc = xmlParseMemory(data_ptr->out, strlen(data_ptr->out)); /* a cleaner way to parse: Tue Jun 10 11:54:44 PDT 2014 */

doc = xmlParseMemory(data_ptr->out, msg_len); /* a cleaner way to parse: Wed Jun 11 17:15:38 MDT 2014 */

//if (doc == NULL) {
if (doc == NULL || doc == 0) {  /* sometimes 0 is returned instead of NULL - we have to accomodate that:  Sat Jun 14 11:32:07 MDT 2014 */  
        fprintf(stderr, "receive_xml(): Failed to parse document!\n");
        //xmlFreeDoc(doc); /* Thu Jun  5 17:50:48 MDT 2014 */
        //fprintf(stdout, "receive_xml(): freed document!\n"); /* Thu Jun  5 17:50:48 MDT 2014 */
        return (-1);
                 }

/*Get the root element node */
root_element = xmlDocGetRootElement(doc);

if (root_element == NULL) { /* Thu Jun  5 17:50:48 MDT 2014 */
        fprintf(stderr, "receive_xml(): root element of the XML doc is NULL!\n");
        xmlFreeDoc(doc);
        doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
        fprintf(stdout, "receive_xml(): freed document!\n");
        return (-1);
                          }


//analyze(filename, root_element);

xmlFreeDoc(doc);

doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */

TotalMessagesReceived++;//count the number of BGP messages received
        return 0;
}


void processStream_2(void) //with valid message
{
 while (terminateFlag != 1) {

data_ptr = malloc(sizeof(data));

if (data_ptr == NULL) { data_ptr = NULL; return; } //KB: have to check memory allocation

strcpy(data_ptr->in, "");
strcpy(data_ptr->out, "");


printf("processStream: about to call receive_xml() \n");

if (receive_xml_2((char *) valid_message, data_ptr) == -1) {


memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));

data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL; //KB: set to NULL -  without setting to NULL telescope was giving seg. faults when stream is interrupted 
// break; /*  Sun May 25 15:53:50 PDT 2014 */
continue; /* continue operating if the message is malformed just skip it. Sun May 25 15:53:50 PDT 2014 */

                                           }

printf("processStream: reached only if receive_xml() returns 0 \n");

memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));
data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL;

          		   }//end of main loop   
}


void processStream_3(void) //with invalid message
{
 while (terminateFlag != 1) {

data_ptr = malloc(sizeof(data));

if (data_ptr == NULL) { data_ptr = NULL; return; } //KB: have to check memory allocation

strcpy(data_ptr->in, "");
strcpy(data_ptr->out, "");


printf("processStream: about to call receive_xml() \n");

if (receive_xml_2((char *) invalid_message, data_ptr) == -1) {


memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));

data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL; //KB: set to NULL -  without setting to NULL telescope was giving seg. faults when stream is interrupted 
// break; /*  Sun May 25 15:53:50 PDT 2014 */
continue; /* continue operating if the message is malformed just skip it. Sun May 25 15:53:50 PDT 2014 */

                                           }

printf("processStream: reached only if receive_xml() returns 0 \n");

memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));
data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL;

          		   }//end of main loop   
}



int main(int argc, char** argv) {
    
    //char evalue [] = "(type = STATUS) | (type = UPDATE)"; 
    //char evalue [] = "(type = STATUS)"; 
    
    //strcpy(expression, evalue);
/*
    printf("%%SUITE_STARTING%% newsimpletest\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (newsimpletest)\n");
    test1();
    printf("%%TEST_FINISHED%% time=0 test1 (newsimpletest) \n");

    printf("%%TEST_STARTED%% test2 (newsimpletest)\n");
    test2();
    printf("%%TEST_FINISHED%% time=0 test2 (newsimpletest) \n");

    printf("%%SUITE_FINISHED%% time=0\n");
*/

    //printf("expression is: %s\n", expression);
    
    //exp_gn = tokenize(expression);
    
    //printf("exp_gn is:%d\n", exp_gn);
    
    //testLoop();
    
    //testSwap();
    
    //testSwap2();
    
    //testDecrement();
    
    //processStreamMock();
    
    //processStream(); //passed test both with 0/-1 input from receive_xml()
    
    processStream_2();
    
    processStream_3();
    
    return (EXIT_SUCCESS);
}
