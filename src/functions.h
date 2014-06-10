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
#define	FUNCTIONS_H

#include "definitions.h"
#include "queuelib.h"

HashTable HT; /* Loop-detection hash table cache */
HashEntry heItem; /* hash table entry item */
Queue XMLQ = NULL; //queue objects
Queue XMLQS[R_NUM]; //array of queue objects
QueueTable QT = NULL; //consolidated data structure - makes XMLQS obsolete

int createQueues(void)
{
    Queue xmlq = NULL;
    int i;
    
    //clear array
    for (i=0; i < R_NUM; i++)
        XMLQS[i] = NULL;
    
    for (i=0; i < R_NUM; i++)
    {
        xmlq = createQueue();       
        XMLQS[i] = xmlq;
        xmlq = NULL;
    }
    return 0;
}

int setupQueueTable(void)
{
    QT = createQueueTable();
    return 0;
}

int destroyQueues(void)
{
    int i;
    
    for (i=0; i < R_NUM; i++)
        destroyQueue(XMLQS[i]);  
    //clear array
    for (i=0; i < R_NUM; i++)
        XMLQS[i] = NULL;
    
    return 0;
}

int removeQueueTable(void)
{
    destroyQueueTable(QT);
    free(QT);
    QT = NULL;
    return 0;
}

int writeQueues(void * item)
{
    int i;
    
    for (i=0; i < R_NUM; i++)
        writeQueue(XMLQS[i], (void *) item);
    return 0;
}

int writeQueueTable(void * item)
{
    int i;
    if (QT != NULL)
    {
    for (i=0; i < R_NUM; i++)
        writeQueue(QT->qtable[i], (void *) item);
    
    return 0;
    } else {
        perror("Queue Table is not initialized!");     
        return -1;
           }
}

static void setTerminateFlag (int sigtype)
{
	terminateFlag = 1;
}

int setSignals(void)
{
//lets install process-wide signal handler to avoid exiting on SIGPIPE signal
	memset(&handler, 0, sizeof(handler));
        if (sigaction(SIGPIPE, NULL, &handler) < 0)
        perror("sigaction() failed for SIGPIPE!");
        else if (handler.sa_handler == SIG_DFL) {
                handler.sa_handler = SIG_IGN;
        if (sigaction(SIGPIPE, &handler, NULL) < 0)
        perror("sigaction() failed for SIGPIPE!");

                                                }   
//Set term handler for SIGTERM like signals arrival
	memset(&term_handler, 0, sizeof(term_handler));
//term_handler.sa_handler = terminate_;//set the corresponding termination routine
term_handler.sa_handler = setTerminateFlag;//set the corresponding termination routine
sigemptyset(&term_handler.sa_mask);
//now add a bunch of signals...
if (sigaddset(&term_handler.sa_mask, SIGTERM) < 0)
        perror("sigaddset() failed!");
if (sigaddset(&term_handler.sa_mask, SIGINT) < 0)
        perror("sigaddset() failed!");
if (sigaddset(&term_handler.sa_mask, SIGQUIT) < 0)
        perror("sigaddset() failed!");

        term_handler.sa_flags = 0;

if (sigaction(SIGTERM, &term_handler, 0) < 0)
        perror("sigaction() failed for SIGTERM!");
if (sigaction(SIGINT, &term_handler, 0) < 0)
        perror("sigaction() failed for SIGINT!");
if (sigaction(SIGQUIT, &term_handler, 0) < 0)
        perror("sigaction() failed for SIGQUIT!");
//block signals in threads
//pthread_sigmask ( SIG_BLOCK, &term_handler.sa_mask, NULL );
        return 0;
}

int InitializeParseEngine(char *exp, int eflag, char *evalue) 
{
	int i, l, k;
        
        resetMatch();//02/17/13
        
        clearExpGlobals(); //05/10/12 clear the exp globals arrays

             if (strlen(evalue) > 4096 )/* 09/12/11 - added extra security check */
		{
		perror("FATAL: Expression length is longer then 4096 characters! Avoiding buffer overrun!");
		exit(-1);
		}

        //fill the exp arrays with data 
	if (eflag == 1) {
	strcpy(exp, evalue);
	mytrim(exp);
			}
	//case: if exp contains ()
	//fill the expg global arrays with data
	if (strrchr(exp, '(') != NULL) {
	complex = true;
        fprintf(logfile, "COMPLEX expression!\n");
				       }
        else {//05/09/12 reset to false
        complex = false;
        fprintf(logfile, "SIMPLE expression!\n");
             }
            
	if (complex == false)
	fill(exp);	

	if (complex == true)		{//start

        //clearExpGlobals(); //05/09/12 clear the exp globals array
        
	exp_gn = tokenize(exp);

        fprintf(logfile, "exp_gn is:%d\n", exp_gn);
        //mytrim the exps
        for (i=0; i < exp_gn; i++) {
        mytrim(expg[i]);
                                   }

        //start logic
        for (i=0; i < exp_gn; i++) {
        l=0;
        if (expg[i][l] == '&' || expg[i][l] == '|') {
                plogic[i] = expg[i][l];
                expg[i][l] = ' ';
                                                    }
                                   }
//end logic
        for (i=0; i < exp_gn; i++) {
        l = strlen(expg[i]);
        l--;
        if (expg[i][l] == '&' || expg[i][l] == '|') {
                if (plogic[i] != '&' && plogic[i] != '|')
                plogic[i] = expg[i][l];
                        else {
                        k=i; k++;
                fprintf(logfile, "K is:%d\n", k);
                plogic[k] = expg[i][l];
                             }

                expg[i][l] = '\0';
                                                   }
                                   }

        //mytrim again
        for (i=0; i < exp_gn; i++) {
        mytrim(expg[i]);
                                   }
        shuffle(exp_gn);
        exp_gn = shuffle2(exp_gn);
					}//end of case: complex == true. ()
	return 0;
}

int ReInitializeParseEngine(int eflag, char *evalue) 
{
	int i, l, k;
        
        resetMatch();//02/17/13
        
        clearExpGlobals(); //05/10/12 clear the exp globals arrays

             if (strlen(evalue) > 4096 )/* 09/12/11 - added extra security check */
		{
		perror("FATAL: Expression length is longer then 4096 characters! Avoiding buffer overrun!");
		exit(-1);
		}

        //fill the exp arrays with data 
	if (eflag == 1) {
	strcpy(expression, evalue);
	mytrim(expression);
			}
	//case: if exp contains ()
	//fill the expg global arrays with data
	if (strrchr(expression, '(') != NULL) {
	complex = true;
        fprintf(logfile, "COMPLEX expression!\n");
				       }
        else {//05/09/12 reset to false
        complex = false;
        fprintf(logfile, "SIMPLE expression!\n");
             }
            
	if (complex == false)
	fill(expression);	

	if (complex == true)		{//start

        //clearExpGlobals(); //05/09/12 clear the exp globals array
        
	exp_gn = tokenize(expression);

        fprintf(logfile, "exp_gn is:%d\n", exp_gn);
        //mytrim the exps
        for (i=0; i < exp_gn; i++) {
        mytrim(expg[i]);
                                   }

        //start logic
        for (i=0; i < exp_gn; i++) {
        l=0;
        if (expg[i][l] == '&' || expg[i][l] == '|') {
                plogic[i] = expg[i][l];
                expg[i][l] = ' ';
                                                    }
                                   }
        //end logic
        for (i=0; i < exp_gn; i++) {
        l = strlen(expg[i]);
        l--;
        if (expg[i][l] == '&' || expg[i][l] == '|') {
                if (plogic[i] != '&' && plogic[i] != '|')
                plogic[i] = expg[i][l];
                        else {
                        k=i; k++;
                fprintf(logfile, "K is:%d\n", k);
                plogic[k] = expg[i][l];
                             }

                expg[i][l] = '\0';
                                                   }
                                   }

        //mytrim again
        for (i=0; i < exp_gn; i++) {
        mytrim(expg[i]);
                                   }
        shuffle(exp_gn);
        exp_gn = shuffle2(exp_gn);
					}//end of case: complex == true. ()
	return 0;
}


int returnip (void) 
{

pptr= hp->h_addr_list;

for (; *pptr!= NULL; pptr++) {
sprintf(ipptr, inet_ntop(AF_INET, *pptr, str, sizeof(str)));
			     }       
return 0;
}


int receive_xml(char *filename, struct xml *data_ptr)
{
xmlDoc *doc = NULL;
xmlNode *root_element = NULL;
int i,j;
char filter[8];
char refilter[9];

int start_len_next;
int msg_len;

rcv = recv(peer_sock, data_ptr->in, xmlStrlen(start_next), MSG_WAITALL);
strcpy(data_ptr->out, data_ptr->in);

start_len_next = strlen(data_ptr->in);
//start_len_next = xmlStrlen(start_next);

//extract the real length
//for (i=21, j=0; i<start_len_next, j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

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


strcpy(data_ptr->in, "");
rcv = recv(peer_sock, data_ptr->in, msg_len - start_len_next, MSG_WAITALL);

strncat(data_ptr->out, data_ptr->in, msg_len - start_len_next); //stronger checking condition in case recv() is fooled...
fprintf(logfile, "now we parse the data_ptr->out_m in memory and get all the elements\n" );

//XML stuff

//doc = xmlReadMemory(data_ptr->out, sizeof(data_ptr->out), "next_bgp_m.xml", NULL, 0);
//doc = xmlReadMemory(data_ptr->out, strlen(data_ptr->out), "buffer.xml", NULL, 0);

doc = xmlParseMemory(data_ptr->out, strlen(data_ptr->out)); /* a cleaner way to parse: Tue Jun 10 11:54:44 PDT 2014 */

if (doc == NULL) {
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


analyze(filename, root_element);

xmlFreeDoc(doc);

doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */

TotalMessagesReceived++;//count the number of BGP messages received
        return 0;
}

int count(char * str)
{

int i;
for (i=0; i < strlen (str); i++) 
	if (str[i] == '.') { 
	return i;
			   }
	return 0;
}


int makereg(char * str, char * pattern)
{

int i, c;
int h=1;
char tmp[INET_ADDRSTRLEN];

memset(tmp, '\0', INET_ADDRSTRLEN);
	tmp[0] = '^';

for (i=0; i < strlen (str); i++) {
	if (isdigit(str[i])) {
c = count(tmp);
if (c == 0)
	tmp[i+1] = str[i];
	else {
	if (isdigit(tmp[c+h])) { 
	h++;
	tmp[c+h] = str[i];
	} else
	tmp[c+h] = str[i];
	     }
			     }

	if (str[i] == '.') {
c = count(tmp);
if (c == 0) {
	tmp[i+1] = '\\';
	tmp[i+2] = str[i];
	    }
	    else{	
	tmp[c+h+1] = '\\';
	tmp[c+h+2] = str[i];
		}
           	           }	
				 }
strcpy (pattern, tmp);
	return 0;
}


int ip_range(char *p1, char *p2, uint32_t pn, uint32_t cn)
{

int i;
uint32_t nnetmask;
struct sockaddr_in sin;
uint32_t ipa;
uint32_t ipb;
uint32_t wildcard_dec;
uint32_t netmask_dec;

//IPv6 stuff
bool ipv6 = false;
int rem, times;
struct sockaddr_in6 sin6;
uint32_t ipa6 [4];
uint32_t ipb6 [4];
uint32_t mask [4]; //raw mask
uint32_t fabmask [4]; //fabricated mask


if (strlen(p2) <= INET6_ADDRSTRLEN && strchr(p2, ':') != NULL)
		ipv6 = true;

if (exactflag == 1) {
if (pn != cn) return -1;
else nnetmask = cn;
		    }
if (moreflag == 1) {
if (pn <= cn) return -1;
else nnetmask = cn;
		   }
if (lessflag == 1) {
if (pn >= cn) return -1;
else nnetmask = pn;
		   }


fprintf(logfile, "ip_range: prefix netmask:%d\n", pn);
fprintf(logfile, "ip_range: cmd netmask:%d\n", cn);
fprintf(logfile, "ip_range: nnetmask:%d\n", nnetmask);

//setting netmask_dec for IPv4 
wildcard_dec = pow(2, (32 - nnetmask)) - 1;
netmask_dec = ~ wildcard_dec;

//fprintf(logfile, "wildcard: %ld\n", wildcard_dec);
//fprintf(logfile, "netmask_dec: %ld\n", netmask_dec);

//setting mask array to all zeroes
        for (i = 0; i < 4; i++)
        mask [i] = 0;
//setting fab mask array to all zeroes
        for (i = 0; i < 4; i++)
        fabmask [i] = 0;

//in case it is IPv6
if (ipv6 == true) {

//setting remainder & times 
rem = nnetmask % 32; 
times = nnetmask / 32; 
fprintf(logfile, "ip_range: rem is:%d\n", rem);
fprintf(logfile, "ip_range: times is:%d\n", times);

//setting mask array 
if (rem == 0 && times > 0) {
        for (i = 0; i < times; i++)
        mask [i] = 32; 
                           }
//setting mask array 
if (rem > 0 && times > 0) {
        for (i = 0; i < times; i++)
        mask [i] = 32;
        mask [i] = rem;
                          }

        for (i = 0; i < 4; i++)
        fprintf(logfile, "ip_range: mask array is:%d\n", mask[i]);

	if (inet_pton(AF_INET6, p1, &sin6.sin6_addr) <= 0) {
	perror("ip_range: p1:error converting IPv6 address!");
	return -1;
	} else ipv6 = true;

ipa6[0] = ntohl(sin6.sin6_addr.s6_addr[0]);
ipa6[1] = ntohl(sin6.sin6_addr.s6_addr[1]);
ipa6[2] = ntohl(sin6.sin6_addr.s6_addr[2]);
ipa6[3] = ntohl(sin6.sin6_addr.s6_addr[3]);

fprintf(logfile, "ip_range: ipa6 0:%d\n", ipa6[0]);
fprintf(logfile, "ip_range: ipa6 1:%d\n", ipa6[1]);
fprintf(logfile, "ip_range: ipa6 2:%d\n", ipa6[2]);
fprintf(logfile, "ip_range: ipa6 3:%d\n", ipa6[3]);

if (inet_pton(AF_INET6, p2, &sin6.sin6_addr) <= 0) {
perror("ip_range: p2:error converting IPv6 address!");
return -1;
} else ipv6 = true;

ipb6[0] = ntohl(sin6.sin6_addr.s6_addr[0]);
ipb6[1] = ntohl(sin6.sin6_addr.s6_addr[1]);
ipb6[2] = ntohl(sin6.sin6_addr.s6_addr[2]);
ipb6[3] = ntohl(sin6.sin6_addr.s6_addr[3]);

fprintf(logfile, "ip_range: ----------------------------------\n");

fprintf(logfile, "ip_range: ipb6 0:%d\n", ipb6[0]);
fprintf(logfile, "ip_range: ipb6 1:%d\n", ipb6[1]);
fprintf(logfile, "ip_range: ipb6 2:%d\n", ipb6[2]);
fprintf(logfile, "ip_range: ipb6 3:%d\n", ipb6[3]);

//lets set the bits in the fab mask accordingly
        for (i = 0; i < 4; i++) {
wildcard_dec = pow(2, (32 - mask[i])) - 1;
netmask_dec = ~ wildcard_dec;
fabmask[i] = netmask_dec;
                                }

        for (i = 0; i < 4; i++)
        fprintf(logfile, "ip_range: fab mask array is:%d\n", fabmask[i]);


for (i = 0; i < 4; i++) {
if  ((ipa6[i] & fabmask[i]) == (ipb6[i] & fabmask[i]))
                ;
        else
                return -1;
                        }

                return 0;

		   } //end of ipv6 == true

//inet_pton(AF_INET, p1, &sin.sin_addr.s_addr);
//inet_pton(AF_INET, p2, &sin.sin_addr.s_addr);

//in case it is IPv4
if (inet_pton(AF_INET, p1, &sin.sin_addr.s_addr) <= 0) {
perror("ip_range: p1:error converting IPv4 address!");
return -1;
} else ipa = ntohl(sin.sin_addr.s_addr);

if (inet_pton(AF_INET, p2, &sin.sin_addr.s_addr) <= 0) {
perror("ip_range: p2:error converting IPv4 address!");
return -1;
} else ipb = ntohl(sin.sin_addr.s_addr);

    fprintf (logfile, "ip_range: p1 content:%s\n",p1);
    fprintf (logfile, "ip_range: p2 content:%s\n",p2);

fprintf(logfile, "ip_range: ------------------\n");

//*
uint32_t a = ipa & netmask_dec;
fprintf(logfile, "ip_range: a: %d\n", a);
uint32_t b = ipb & netmask_dec;
fprintf(logfile, "ip_range: b: %d\n", b);
//*/

if  ((ipa & netmask_dec) == (ipb & netmask_dec))
                return 0;
        else
                return -1;
}

int tuple_populate(char *tuple_str)
{
        int ic=0;
        while (tuple_str != NULL)
        {   
        sprintf(tuple[ic++], tuple_str);
        tuple_str = strtok(NULL, " ");
        }  
        return 0;
}

int get_element(xmlNode * a_node, char *element_name, char operator, char *value)
{
	xmlNode *cur_node = NULL;
	int i, j;
	static int f = 0;
	int numeric_v, num;
	bool string = false;

	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
         //fprintf(logfile, "get_element: Element name: %s\n", cur_node->name);
         //fprintf(logfile, "get_element: value: %s\n", cur_node->content);
		if (f == 1 && xmlStrEqual(cur_node->name, (xmlChar *) "text" ) == 1) {
		numeric_v = atoi((char *)cur_node->content);
         //fprintf(logfile, "get_element: numeric_v is: %d\n", numeric_v);
		num = atoi(value);

		//test whether the value is a string
		if (isalpha(value[0]) != 0)
			string = true;

		if (operator == '=') {
			if (string != true) {
			if (numeric_v == num) {
          fprintf(logfile, "get_element: = FOUND!%s\n", value);
			f = 0;
			return(0);
					      }
			else {
			f = 0;
			return(-1);
			     }
					    }
			if (string == true) {
			if (strcmp ((char *)cur_node->content, value) == 0) {
          fprintf(logfile, "get_element: = FOUND!%s\n", value);
			f = 0;
			return(0);
					      				    }
			else {
			f = 0;
			return(-1);
			     }
					    }
				     }
		if (operator == '>') {
			if (numeric_v > num) {
          fprintf(logfile, "get_element: > FOUND!%s\n", value);
			f = 0;
			return(0);
					     }
			else {
			f = 0;
			return(-1);
			     }
				     }
		if (operator == '<') {
			if (numeric_v < num) {
          fprintf(logfile, "get_element: < FOUND!%s\n", value);
			f = 0;
			return(0);
					     }
			else {
			f = 0;
			return(-1);
			     }
				     }
		if (operator == '!') {
			if (string != true) {
			if (numeric_v != num) {
          fprintf(logfile, "get_element: ! FOUND!%s\n", value);
			f = 0;
			return(0);
					      }
			else {
			f = 0;
			return(-1);
			     }
				     	    }
			if (string == true) {
                        if (strcmp ((char *)cur_node->content, value) != 0) {
          fprintf(logfile, "get_element: ! FOUND!%s\n", value);
                        f = 0;
                        return(0);
                                                                            }
                        else {
                        f = 0;
                        return(-1);
                             }
                                            }

				     }
			f = 0;
			return(-1);
			    							     }
	if (xmlStrEqual(cur_node->name, (xmlChar *) element_name) == 1){
          //fprintf(logfile, "get_element: Element name: %s\n", cur_node->name);
          fprintf(logfile, "get_element: operator is: %c\n", operator);
		if (f == 0)
		    f = 1;
								       }
				 }
	if (get_element(cur_node->children, element_name, operator, value) == 0){
                        return 0;
                                                                           	}

											    }//end of for loop

				return -1;

}

int get_attribute(xmlNode * a_node, char *attribute_name, char operator, char *value)
{
	xmlNode *cur_node = NULL;
	xmlChar * buf;
	int i, j;
	static int f = 0;
	int numeric_v, num;
	bool string = false;

	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
        // fprintf(logfile, "get_attribute: Element name: %s\n", cur_node->name);
        // fprintf(logfile, "get_attribute: value: %s\n", cur_node->content);
		/*
		if (f == 1 && xmlStrEqual(cur_node->name, (xmlChar *) "text" ) == 1) 
		*/
		if (f == 1)  {
	if (cur_node->properties != NULL) {
        if (cur_node->properties->type == XML_ATTRIBUTE_NODE) {
	
		buf = xmlGetProp(cur_node, (xmlChar *) attribute_name);
		//numeric_v = atoi((char *)cur_node->content);

                if (buf == NULL)
			continue;

		numeric_v = atoi((char *)buf);
		num = atoi(value);

		//test whether the value is a string
		if (isalpha(value[0]) != 0)
			string = true;

		if (operator == '=') {
			if (string != true) {
			if (numeric_v == num) {
          fprintf(logfile, "get_attribute: = FOUND!%s\n", value);
			f = 0;
			return(0);
					      }
			else {
			f = 0;
			return(-1);
			     }
					    }
			if (string == true) {
			if (strcmp ((char *)buf, value) == 0) {
          fprintf(logfile, "get_attribute: = FOUND!%s\n", value);
			f = 0;
			return(0);
					      				    }
			else {
			f = 0;
			return(-1);
			     }
					    }
				     }
		if (operator == '>') {
			if (numeric_v > num) {
          fprintf(logfile, "get_attribute: > FOUND!%s\n", value);
			f = 0;
			return(0);
					     }
			else {
			f = 0;
			return(-1);
			     }
				     }
		if (operator == '<') {
			if (numeric_v < num) {
          fprintf(logfile, "get_attribute: < FOUND!%s\n", value);
			f = 0;
			return(0);
					     }
			else {
			f = 0;
			return(-1);
			     }
				     }
		if (operator == '!') {
			if (string != true) {
			if (numeric_v != num) {
          fprintf(logfile, "get_attribute: ! FOUND!%s\n", value);
			f = 0;
			return(0);
					      }
			else {
			f = 0;
			return(-1);
			     }
				     	    }
			if (string == true) {
                        if (strcmp ((char *)buf, value) != 0) {
          fprintf(logfile, "get_attribute: ! FOUND!%s\n", value);
                        f = 0;
                        return(0);
                                                              }
                        else {
                        f = 0;
                        return(-1);
                             }
                                            }

				     }
			f = 0;
                    xmlFree(buf);
			return(-1);
										}//end of type == XML_ATTRIBUTE_NODE	
			    							     }//end of properties != NULL
										}//end of if (f == 1)
											
		if (f == 0)
		    f = 1;
								       
				 }//end of type != 0
	if (get_attribute(cur_node->children, attribute_name, operator, value) == 0){
                        return 0;
                                                                           	    }

											    }//end of for loop
				return -1;
}

int fill(char * exp)
{
int l = 0;
int ll = 1;
int np = 1;
int i;
char tmp[256];
char *tuple_str;

//we should preserve the original expg[] arrays and work with tmp copy
strcpy (tmp, exp);

for (i=0; i < strlen(exp); i++) {
if (isspace(exp[i]))
np++;
                                }
fprintf(logfile, "np is:%d\n", np);

fprintf(logfile, "exp is:%s\n", exp);

for (i=0; i < np; i++) {
//tuple_str = strtok(exp, " ");//swap with tmp copy of expression
tuple_str = strtok(tmp, " ");
tuple_populate(tuple_str);
}

fprintf(logfile, "tuple_str is:%s\n", tuple_str);

for (i=0; i < np; i++){
fprintf (logfile, "%s\n",tuple[i]);
                      }

for (i=0; i < 0; i++);

while (i < np) {
strcpy(element[l], tuple[i]);
i++;
operator[l] = tuple[i][0];
i++;
strcpy(value[l], tuple[i]);
i++;
logic[ll] = tuple[i][0];
i++;

l++;
ll++;
              }
logic[0] = logic[1];
			return 0;
}

int get_prefix(xmlNode * a_node, char *element_name, char operator, char *value)
{
    	xmlNode *cur_node = NULL;

        xmlChar buf[2*512];
	char str[2*512];
	char ip[INET_ADDRSTRLEN];
	char cidr[INET_ADDRSTRLEN];

	int i, j, z, r; 

	int dot = 0;
	int sl = 0;
	uint32_t prefix_netmask;

uint32_t netmask;
char tmp[INET6_ADDRSTRLEN];
char dmp[INET6_ADDRSTRLEN];

//setting operator flag 
switch (operator)
{
	case 'e':
	exactflag = 1;
	break;
	case 'l': 
        lessflag = 1; 
        break;
        case 'm': 
        moreflag = 1; 
        break;
	default:
        abort ();
}

fprintf(logfile, "get_prefix: operator is:%c\n", operator);

//setting netmask
        memset(tmp, '\0', INET6_ADDRSTRLEN);
        memset(dmp, '\0', INET6_ADDRSTRLEN);

for (i=0; i < strlen (value); i++) {
        if (value[i] == '/') {
        tmp[0] = value[i+1];
                if (isdigit(value[i+2]))
        tmp[1] = value[i+2];
                //ipv6 check
                if (strchr(value, ':') != NULL)
                        if (isdigit(value[i+3]))
        tmp[2] = value[i+3];

                break;
                               }
                                   }

fprintf(logfile, "get_prefix:tmp is:%s\n", tmp);
        netmask = atol(tmp);
fprintf(logfile, "get_prefix:netmask:%d\n", netmask);

//setting net
        memset(tmp, '\0', INET6_ADDRSTRLEN);

for (i=0; i < strlen (value); i++) {
        if (value[i] == '/') {
        strncpy(tmp, value, i);
                break;
                               }
                                     }

fprintf(logfile, "get_prefix:tmp is:%s\n", tmp);
strcpy(dmp, tmp);
fprintf(logfile, "get_prefix:dmp is:%s\n", dmp);
fprintf(logfile, "get_prefix:value is:%s\n", value);

//function processing starts here

    for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

	if (cur_node->type != 0) {
            fprintf(logfile, "get_prefix:node type: Element name: %s\n", cur_node->name);
            fprintf(logfile, "get_prefix:node content: %s\n", cur_node->content);

        if (strcmp(element_name, "PREFIX" ) == 0) {
        if (xmlStrEqual(cur_node->name, (xmlChar *) "text" ) == 1 && xmlStrlen(cur_node->content) <= INET6_ADDRSTRLEN ) {
            fprintf(logfile, "get_prefix:search () node content: %s\n", cur_node->content);
	xmlStrPrintf(buf, xmlStrlen(cur_node->content)+1, cur_node->content);
        //fprintf(logfile, "xml  buffer is:  %s\n", buf);

	strcpy (str, (char *) buf);
        fprintf(logfile, "get_prefix:char str is: %s\n", str);

	for (z=0; z < strlen (str); z++)
        if (str[z] == '.')
                dot++;

//setting net
	//if (dot == 2) { //08/16/11: it seems the PREFIX format has changed - now we need 3 dots in IP
	if (dot == 3) { //08/16/11: 3 dots in IP
for (z=0; z < strlen (str); z++) {
        if (str[z] == '/') {
		sl = 1;
fprintf(logfile, "get_prefix:set sl to:%d\n", sl);
	memset(ip, '\0', INET_ADDRSTRLEN);
        strncpy(ip, str, z);
	//strcat(ip, ".0");//08/16/11:it seems the PREFIX format has changed - since 3 dots are in IP - no need to append .0 at the end
        break;
        		   }
				}
              	      }

fprintf(logfile, "get_prefix:dot is:%d\n", dot);

//setting netmask
        memset(cidr, '\0', INET_ADDRSTRLEN);

for (z=0; z < strlen (str); z++) {
        if (str[z] == '/') {
        cidr[0] = str[z+1];
                if (isdigit(str[z+2]))
        cidr[1] = str[z+2];
                break;
                            }
                                 }

fprintf(logfile, "get_prefix:cidr is:%s\n", cidr);
        prefix_netmask = atol(cidr);
fprintf(logfile, "get_prefix:prefix_netmask:%d\n", prefix_netmask);

	if (sl == 1) {
        fprintf(logfile, "get_prefix:char ip is: %s\n", ip);
if ((r = ip_range(ip, dmp, prefix_netmask, netmask)) == 0) {
        fprintf(logfile, "get_prefix:%d: string matched!\n", r);
        } else if (r == -1)
        fprintf(logfile, "get_prefix:string did not match!\n");

        if (r == 0){
			if (match == -1){
			match = 0;
        fprintf(logfile, "get_prefix:%d: inside search() - testing match!\n", match);
			return 0;
					}
		   }
		     } //end of sl == 1
					} //end of ->name == text

					
			} //end of PREFIX == 0


        if (xmlStrEqual(cur_node->name, (xmlChar *) element_name ) == 1)
	if (xox == -1)
           xox=i;
        if (xmlStrEqual(cur_node->content, (xmlChar *) value ) == 1)
	if (zoz == -1)
            zoz=j;

				 } //end of ->type != 0

        //    fprintf(logfile, "RETURN!\n");

	if (xox != -1 && zoz != -1) {
            fprintf(logfile, "get_prefix: found %s =	%s\n\n", element_name, content);
			return 0;
      		     		    }

if ( get_prefix(cur_node->children, element_name, operator, value ) == 0) {
			if (match == 0)
			return 0;
									  }
    								 		       } //end of for loop
	return -1;
}


//the same as analyze but without writing to a file
//used in complex expression handling
int analyse (xmlNode *root_element)
{
int exp_n = 0;
int cycle_and[256];
int cycle_or[256];
bool oracle_and = true;
bool oracle_or = false;
bool oracle = false;

int and_count = 0;
int or_count = 0;

int i;
//lets fill the logic arrays with zeroes initially
for (i=0; i < 256; i++){
        cycle_and[i] = 0;
        cycle_or[i] = 0;
                       }

//lets count the number of expressions
for (i=0; i < 0; i++);
while (logic[i] == '&' || logic[i] == '|'){
        i++; exp_n++;
                                         }

if (exp_n == 0)
if (strlen(element[0]) != 0 && operator[0] != 0 && strlen(value[0]) != 0)
        exp_n = 1;

fprintf(logfile, "exp_n is:%d\n", exp_n);

//lets count the AND and OR
for (i=0; i < exp_n; i++) {
        if (logic[i] == '&')
        and_count++;
        if (logic[i] == '|')
        or_count++;
                          }


for (i=0; i < exp_n; i++)//loop through defined tags/attributes of the expression
                {
if (strcmp(element[i], "PREFIX") == 0) {//operate on specified network prefix
if (get_prefix(root_element, element[i], operator[i], value[i]) == 0){
                if (logic[i] == '&') {
                cycle_and[i] = 1;
                                     }
                if (logic[i] == '|') {
                cycle_or[i] = 1;
                                     }
                if (exp_n == 1) oracle = true; //if there is just one exp
                                                                     }
                                       }
		else if (strcmp(element[i], "length") == 0 || strcmp(element[i], "version") == 0 || strcmp(element[i], "type") == 0 || strcmp(element[i], "value") == 0 || strcmp(element[i], "code") == 0 || strcmp(element[i], "timestamp") == 0 || strcmp(element[i], "datetime") == 0 || strcmp(element[i], "precision_time") == 0 || strcmp(element[i], "withdrawn_len") == 0 || strcmp(element[i], "path_attr_len") == 0 || strcmp(element[i], "label") == 0) {//operate on specified attributes
if (get_attribute(root_element, element[i], operator[i], value[i]) == 0){	
		if (logic[i] == '&') {
		cycle_and[i] = 1;
				     }
		if (logic[i] == '|') {
		cycle_or[i] = 1;
				     }
		if (exp_n == 1) oracle = true; //if there is just one exp
								      	}
	 	       }
                else {
if (get_element(root_element, element[i], operator[i], value[i]) == 0){//operate on elements
                if (logic[i] == '&') {
                cycle_and[i] = 1;
                                     }
                if (logic[i] == '|') {
                cycle_or[i] = 1;
                                     }
                if (exp_n == 1) oracle = true; //if there is just one exp
                                                                      }
		     }	
                }//end of for loop
fprintf(logfile,"count AND: %d\n", and_count);
fprintf(logfile,"count OR: %d\n", or_count);

for (i=0; i < and_count; i++)
        fprintf(logfile,"cycle array AND: %d\n", cycle_and[i]);
//for (i=0; i < or_count; i++)
for (i=0; i < 256; i++)
        if (cycle_or[i] == 1)
        fprintf(logfile,"cycle array OR: %d\n", cycle_or[i]);

//for (i=0; i < and_count; i++)
for (i=0; i < exp_n; i++) {
        if (cycle_and[i] != 1) {
        oracle_and = false;
        break;
                               }
                              }
for (i=0; i < exp_n; i++) {
        if (cycle_or[i] == 1) {
        oracle_or = true;
        break;
                              }
                          }
if (oracle_and == true || oracle_or == true || oracle == true)
                                {
        fprintf(logfile,"analyse returned true!\n");
		return 0;
                                }

		return -1;
}

int clearExpGlobals(void)
{
    int i = 0;
    
    memset(expression, '\0', sizeof(expression));
    
    memset(truth, '\0', sizeof(truth));
    
    for (i=0; i < 256; i++){
                truth[i] = 0;
                           }
    
    memset(plogic, '\0', sizeof(plogic));
    
    for (i=0; i < 256; i++){
                plogic[i] = '\0';
                           }
    
    for (i=0; i < 256; i++) {
		memset(expg[i], '\0', sizeof(expg[i]));
		strcpy(expg[i], "");
                        } 
    
    //clear the globals
    for (i=0; i < 256; i++) {
        memset(element[i], '\0', sizeof(element[i]));
        operator[i] = '\0';
        memset(value[i], '\0', sizeof(value[i]));
        logic[i] = '\0';
                            }
    
    for (i=0; i < 1024; i++) {
        memset(tuple[i], '\0', sizeof(tuple[i]));
                             }	
    return 0;
}

//divide the exp based on ()
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

int harvest(char *exp, xmlNode *root_element)
{
int i;
//clear the globals
for (i=0; i < 256; i++) {
memset(element[i], '\0', sizeof(element[i]));
operator[i] = '\0';
memset(value[i], '\0', sizeof(value[i]));
logic[i] = '\0';
memset(tuple[i], '\0', sizeof(tuple[i]));
		   	}		
//fill the globals
    fill(exp);
if (analyse(root_element) == 0)
	return 0;
	else
	return -1;
}

int mytrim(char *str)
{
	int i,l;
	char tmp[256];
	char tm[256];

	memset(tmp, '\0', sizeof(tmp));
	memset(tm, '\0', sizeof(tm));

	i=0; l = 0;
	//remove blanks from the beginning
	while (str) {
		if (isspace(str[i]))
			i++;	
			else break;
		        }
	while (i < strlen(str)) {
		tmp[l] = str[i];
			i++;
			l++;
				}
	strcpy(str, tmp);
	//remove blanks from the end
	i = strlen(str);
	i--; //skip the ending \0

	while (str) {
		if (isspace(str[i])) {
			i--;	
		         	     }
			else break;
		    }
	l=i;

	for (i=0; i<=l; i++) 
		tm[i] = str[i];
	strcpy(str, tm);
	return 0;
}

int shuffle(int exp_gn)
{
int i, k;
char exp_tmp[256][256];

for (i=0; i < 256; i++){
memset(exp_tmp[i], '\0', sizeof(exp_tmp[i]));
                       }

	for (i=0, k=0; i < exp_gn; i++) {
	if (strlen(expg[i]) != 0){
		strcpy(exp_tmp[k], expg[i]);
		k++;
				 }
					}
	for (i=0; i < exp_gn; i++) {
		memset(expg[i], '\0', sizeof(expg[i]));
		strcpy(expg[i], exp_tmp[i]);
		        	   }
	return 0;
}

int shuffle2(int exp_gn)
{
int i, k;
char tmp[256];

for (i=0; i < 256; i++){
tmp[i] = '\0';
                       }

		if (plogic[0] == '\0' && plogic[1] != '\0')
			plogic[0] = plogic[1];
		if (plogic[1] == '\0' && plogic[0] != '\0')
			plogic[1] = plogic[0];

	for (i=0, k=0; i < exp_gn; i++) {
	if (plogic[i] != '\0'){
		tmp[k] = plogic[i];
		k++;
			      }
					}
	for (i=0; i < exp_gn; i++) {
		plogic[i] = '\0';
		plogic[i] = tmp[i];
		        	   }
	return k;
}

int getPeerAddress (const struct sockaddr *addr, char *ip)
{
    const unsigned char *rawaddr = NULL;
    char printable[80];	
    const char *peer = NULL;

    if (addr->sa_family == AF_INET6) {
        const struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;
        rawaddr = (const unsigned char *)&addr6->sin6_addr;
    }
    else if (addr->sa_family == AF_INET) {
        const struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
        rawaddr = (const unsigned char *)&addr4->sin_addr.s_addr;
    }
    if (rawaddr != NULL)
        peer = inet_ntop(addr->sa_family, rawaddr, printable, sizeof(printable));
    if (peer == NULL)
        peer = "(unknown)";

	strcpy (ip, peer);
	return 0;
}

int DSMSwrite_element(char *filename, xmlNode * a_node)
{
	xmlNode *cur_node = NULL;
	//char dataFile[] = "TeleScope.dat";
	char elementNames[16][256]; 
	static char elementValues[16][32]; 
	static int elementIndeces[16]; 
	
	int fd;
	int i, j, z;
	static int f = 0;//found variable
	static int index = 0;
	static int once = 0;
	static int stop = 0;

	if (once == 0)	{
for (i=0; i < 16; i++)
	{
memset(elementNames[i], '\0', sizeof(elementNames[i]));
memset(elementValues[i], '\0', sizeof(elementValues[i]));
elementIndeces[i] = -1;
	}
	once = 1;
	stop = 0;
			}

	if (stop == 0) ; else return 0; //if stop is true - return immediately

	//DST_ADDR DST_AS PREFIX
	//these are the elements we are extracting
	strcpy (elementNames[0], "SRC_ADDR"); 
	strcpy (elementNames[1], "SRC_AS");
	strcpy (elementNames[2], "DST_ADDR"); 
	strcpy (elementNames[3], "DST_AS");
	strcpy (elementNames[4], "ORIGIN"); 
	strcpy (elementNames[5], "MULTI_EXIT_DISC"); 
	strcpy (elementNames[6], "PREFIX"); 
	strcpy (elementNames[7], "OCTETS"); //we put the element that is always present at the end of the regular BGP message at the end to ensure that we traverse all the elements - if it is found it signifies the end of the message

	fd = open (filename, O_CREAT | O_APPEND | O_RDWR, S_IRWU);

		if (elementIndeces[7] == 0) {//if the last element is set to filled (0) it's time to stop & write data
         fprintf(logfile, "write_element: done collecting key elements.\n");
				for (z=0; z < 8; z++) {
				if (strlen(elementValues[z]) != 0)
				{
		write(fd, elementValues[z], strlen(elementValues[z]));
		if (z != 7)
		write(fd, ",", strlen(","));
				} else {
		//write(fd, "NULL", strlen("NULL"));
		write(fd, "-1", strlen("-1"));
		if (z != 7)
		write(fd, ",", strlen(","));
				       }
						      }
		//put the separator per tuple
		write(fd, "\n", strlen("\n"));
		once = 0;
		stop = 1;
				close(fd);
				return 0;
				 	    }

	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
		if (f == 1 && xmlStrEqual(cur_node->name, (xmlChar *) "text" ) == 1) {
         fprintf(logfile, "write_element: Element name: %s\n", cur_node->name);
         fprintf(logfile, "write_element: value: %s\n", cur_node->content);

		if (strlen((char *) cur_node->content) <= 32)
	 strcpy (elementValues[index], (char *) cur_node->content); 
			else
	 strncpy (elementValues[index], (char *) cur_node->content, 32); 

         fprintf(logfile, "write_element: ElementValues: %s\n", elementValues[index]);
		elementIndeces[index] = 0;//set to filled
		if (f == 1)
                    f = 0;
								     	   	     }
				for (z=0; z < 16; z++) {
	if (xmlStrEqual(cur_node->name, (xmlChar *) elementNames[z]) == 1){
          fprintf(logfile, "write_element: Element name: %s\n", cur_node->name);
          fprintf(logfile, "write_element: value: %s\n", cur_node->content);
		if (f == 0)
                    f = 1;
			index = z;//set Element index
			break;
								       	  }
							}
				 }

	if (stop == 0)
	DSMSwrite_element(filename, cur_node->children); 
											    }//end of for loop
				close(fd);
				return 0;
}

/* diagnostic routine  */
int firstMessage(void)
{
char ms[8];//message length buffer
char mms[9];//message length buffer
      
int i,j;//dumb variables...
    
buf_ptr = malloc(sizeof(buf));
bgp_m_ptr = malloc(sizeof(bgp_m));

if (buf_ptr != NULL && bgp_m_ptr != NULL) 
{
rcv = recv(peer_sock, buf_ptr, xmlStrlen(start), MSG_WAITALL);
//fprintf(logfile, "start is:%s\n", buf_ptr);

strcpy(bgp_m_ptr, buf_ptr);
//fprintf(logfile, "bgp_m is:%s\n", bgp_m_ptr);

start_len = strlen(buf_ptr);
//fprintf(logfile, "start_len is:%i\n", start_len);

//extract the real length
//for (i=26, j=0; i<start_len, j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

/* avoid left-hand operand of comma expression has no effect [-Wunused-value] in gcc 4.8+ */
for (i=26, j=0; j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */
      
//fprintf(logfile, "i:%i	%c\n", i,  buf_ptr[i]);
	ms[j] = buf_ptr[i]; 
			  		      }
strncpy(mms, ms, 7);
mms[7] = ms[7];
mms[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

//fprintf(logfile, "mms is:%s\n", mms);

mesg_len = atoi(mms);
//fprintf(logfile, "mesg_len is:%i\n", mesg_len);

rcv = recv(peer_sock, buf_ptr, mesg_len - start_len+5, MSG_WAITALL);
//fprintf(logfile, "end is:%s\n", buf_ptr);

strcat(bgp_m_ptr, buf_ptr);
//fprintf(logfile, "bgp_m is:%s\n", bgp_m_ptr);

//lets remove the <xml> tag
memset(bgp_m_ptr, ' ', 5);
//fprintf(logfile, "bgp_m is:%s\n", bgp_m_ptr);

fprintf(logfile, "now we parse the bgp_m in memory and get all the elements\n" );

//XML stuff

LIBXML_TEST_VERSION

//doc = xmlReadMemory(bgp_m_ptr, sizeof(bgp_m), "bgp_m.xml", NULL, 0);
doc = xmlReadMemory(bgp_m_ptr, strlen(bgp_m_ptr), "buffer.xml", NULL, 0);

if (doc == NULL) {
        fprintf(stderr, "Failed to parse document\n");
        return -1;
    		 }

/*Get the root element node */
root_element = xmlDocGetRootElement(doc);

if (root_element == NULL) { /* Fri Jun 6 15:27:46 MDT 2014 */
        fprintf(stderr, "receive_xml(): root element of the XML doc is NULL!\n");
        xmlFreeDoc(doc);
        doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
        fprintf(stdout, "receive_xml(): freed document!\n");
        return (-1);
                          }

analyze(filename, root_element);

//free the document
xmlFreeDoc(doc);
doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
    
//this is how we try to free mem
memset(buf_ptr, '\0', sizeof(buf));
memset(bgp_m_ptr, '\0', sizeof(bgp_m));
strcpy(buf_ptr, "");
strcpy(bgp_m_ptr, "");
buf_ptr =  realloc(buf_ptr, sizeof(char));
bgp_m_ptr =  realloc(bgp_m_ptr, sizeof(char));

if (buf_ptr != NULL)
        free(buf_ptr);
if (bgp_m_ptr != NULL)
        free(bgp_m_ptr);

//set to NULL
buf_ptr = NULL;
bgp_m_ptr = NULL;
} else {perror("malloc failed!"); exit(-1);}

return 0;
}

/* diagnostic routine  */ 
int secondMessage(void)
{
    int i,j;//dumb variables...
    char ms[8];//message length buffer
    char mms[9];//message length buffer

    //fprintf(logfile, "start_next len is: %i bytes\n", xmlStrlen(start_next));
rcv = recv(peer_sock, buf, xmlStrlen(start_next), MSG_WAITALL);
//fprintf(logfile, "buf is:%s\n", buf);

strcpy(bgp_m, buf);
//fprintf(logfile, "next bgp_m is:%s\n", bgp_m);

start_len_next = strlen(buf);
//start_len_next = xmlStrlen(start_next);
//fprintf(logfile, "start_len_next is:%i\n", start_len_next);

//extract the real length
// for (i=21, j=0; i<start_len_next, j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

/* avoid left-hand operand of comma expression has no effect [-Wunused-value] in gcc 4.8+ */
for (i=21, j=0; j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */

//fprintf(logfile, "i:%i    %c\n", i,  buf[i]);
        ms[j] = buf[i];
                                              	   }
strncpy(mms, ms, 7);
mms[7] = ms[7];
mms[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

//fprintf(logfile, "mms is:%s\n", mms);

mesg_len = atoi(mms);
//fprintf(logfile, "mesg_len is:%i\n", mesg_len);

rcv = recv(peer_sock, buf, mesg_len - start_len_next, MSG_WAITALL);
//fprintf(logfile, "end is:%s\n", buf);
//fprintf(logfile, "len of end is:%d\n", strlen(buf));

strcat(bgp_m, buf);
//fprintf(logfile, "bgp_m is:%s\n", bgp_m);

fprintf(logfile, "now we parse the bgp_m in memory and get all the elements\n" );

//XML stuff

doc = xmlReadMemory(bgp_m, strlen(bgp_m), "buffer.xml", NULL, 0);

if (doc == NULL) {
        fprintf(stderr, "secondMessage(): Failed to parse document\n");
        return -1;
                 }

/*Get the root element node */
root_element = xmlDocGetRootElement(doc);

if (root_element == NULL) { /* Fri Jun 6 15:27:46 MDT 2014 */
        fprintf(stderr, "secondMessage(): root element of the XML doc is NULL!\n");
        xmlFreeDoc(doc);
        doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
        fprintf(stdout, "secondMessage(): freed document!\n");
        return (-1);
                          }    

//call the magic function
analyze(filename, root_element);

xmlFreeDoc(doc);
doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */

strcpy(buf, "");
strcpy(bgp_m, "");

memset(buf, '\0', sizeof(buf));
memset(bgp_m, '\0', sizeof(bgp_m));

fprintf(logfile, "\n\n");
fprintf(logfile, "now calling receive_xml\n\n" );
return 0;
}

int establishPeerConnection(void)
{
    int v = 1;
    //build address data structure
peer_sin.sin_family = AF_INET;
/* convert port number to network byte order */ 
if (port != 0)
peer_sin.sin_port = htons(port);
	else
peer_sin.sin_port = htons(SERVER_PORT);
/* convert IP address in ASCII strings to network byte order binary value */ 
inet_pton(AF_INET, ipptr, &peer_sin.sin_addr.s_addr);

/*zero the rest of the struct*/ 
// memset(&peer_sin.sin_zero, '\0', sizeof(&peer_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

/* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
memset(&peer_sin.sin_zero, '\0', sizeof( * peer_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

memcpy(&hp->h_addr, &peer_sin.sin_addr, sizeof(&hp->h_length));

//create TCP socket to the peer

 if ((peer_sock = socket (PF_INET, SOCK_STREAM, 0)) < 0) {
	perror("can't open a peer socket!");
	exit(1);
	}

len = sizeof(peer_sin);

//lets reuse the socket!
setsockopt(peer_sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));

//connect to the peer/(server)
 if (connect(peer_sock, (struct sockaddr*)&peer_sin, sizeof(peer_sin)) < 0) {
	perror("can't connect to the XML data stream publisher!\n");
	close(peer_sock);
	exit(1);
	} else {
	fprintf(stdout, "connected to the XML data stream publisher.\n");
	fprintf(logfile, "connected to the XML data stream publisher.\n");
	}
        return 0;
}

int prepareServerSocket(void)
{
//build server address data structure
server_sin.sin_family = AF_INET;
server_sin.sin_addr.s_addr = INADDR_ANY;
server_sin.sin_port = htons(SERVER_PORT);

// memset(&server_sin.sin_zero, '\0', sizeof(&server_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

/* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
memset(&server_sin.sin_zero, '\0', sizeof( * server_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */


if ((server_sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("can't open a socket!");
        exit(1);
        }

//lets reuse the socket!
int v = 1;
setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));

if ((bind(server_sock, (struct sockaddr *)&server_sin, sizeof(server_sin))) < 0) {
        perror("can't bind to a socket!");
        exit(1);
}

//lets put the server socket into
//listening mode to accept client requests
if (listen(server_sock, R_NUM) < 0 ) {
        perror("can't start listening on a socket!");
        exit(1);
        			     }
        return 0;
}

int prepareCLIThread(void)
{
    int x = 1;
    //build status address data structure
cli_sin.sin_family = AF_INET;
cli_sin.sin_addr.s_addr = INADDR_ANY;
cli_sin.sin_port = htons(CLI_PORT);

// memset(&cli_sin.sin_zero, '\0', sizeof(&cli_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

/* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
memset(&cli_sin.sin_zero, '\0', sizeof( * cli_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */


if ((cli_sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("can't open a CLI socket!");
        exit(1);
        }
//lets reuse the socket!
setsockopt(cli_sock, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x));

if ((bind(cli_sock, (struct sockaddr *)&cli_sin, sizeof(cli_sin))) < 0) {
        perror("can't bind to a CLI socket!");
        exit(1);
}

//lets put the Status socket into listening mode to accept Status requests
if (listen(cli_sock, 1) < 0 ) {
        perror("can't start listening on a CLI socket!");
        exit(1);
                                 }
return 0;
}

int prepareStatusThread(void)
{
    int x = 1;
    //build status address data structure
status_sin.sin_family = AF_INET;
status_sin.sin_addr.s_addr = INADDR_ANY;
status_sin.sin_port = htons(STATUS_PORT);

// memset(&status_sin.sin_zero, '\0', sizeof(&status_sin.sin_zero)); /* Wed May 21 11:33:42 PDT 2014 */

/* suppress the [-Wsizeof-pointer-memaccess] error in gcc 4.8+ by dereferencing a value */
memset(&status_sin.sin_zero, '\0', sizeof( * status_sin.sin_zero));   /* Wed May 21 11:33:42 PDT 2014 */


if ((status_sock = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("can't open a Status socket!");
        exit(1);
        }
//lets reuse the socket!
setsockopt(status_sock, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(x));

if ((bind(status_sock, (struct sockaddr *)&status_sin, sizeof(status_sin))) < 0) {
        perror("can't bind to a Status socket!");
        exit(1);
}

//lets put the Status socket into listening mode to accept Status requests
if (listen(status_sock, 1) < 0 ) {
        perror("can't start listening on a Status socket!");
        exit(1);
                                 }
return 0;
}

void launchClientsThreadPool(void)
{
// Get the default attributes
   pthread_attr_init(&attr);
   //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//does not really help
   //lets create  Readers thread pool
if (( tidR = (pthread_t *)  calloc(R_NUM, sizeof(pthread_t))) == NULL ) {
                perror("failed to allocate space for Reader thread IDs!");
			free(tidR);
                        exit(-1);
                                                                         }

        /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */   
for(tindex = 0; tindex < R_NUM; tindex++) {
      if (pthread_create(&tidR[tindex],&attr,Reader,(void *) server_sock) != 0) {
		perror("failed to create a client handling Reader thread!");
			free(tidR);
                        exit(-1);
                                                                                }

        if (pthread_detach(tidR[tindex]) != 0) {
        perror("Reader thread failed to detach!");
			free(tidR);
                        exit(-1);
                                               }
                                          }
        fprintf(stdout, "Clients Thread Pool started. Listening on port %d \n", SERVER_PORT);
        fprintf(logfile, "Clients Thread Pool started. Listening on port %d \n", SERVER_PORT);
}

void launchStatusThread(void)
{
    //Launch the Status Thread
        /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */       
if (pthread_create(&StatusThreadID,NULL,StatusThread,(void *) status_sock) != 0) {
                perror("failed to create a Status Thread!");
                        exit(-1);
                                                                         	 }
}

void launchCLIThread(void)
{
    //Launch the Status Thread
        /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */       
if (pthread_create(&CLIThreadID,NULL,CLIThread,(void *) cli_sock) != 0) {
                perror("failed to create a CLI Thread!");
                        exit(-1);
                                                                	}
}

void launchFileReaderThread(void)
{
    //Launch the FileWriter Thread
        /* the last argument to pthread_create must be passed by reference as a pointer cast of type void. NULL may be used if no argument is to be passed. */       
if (pthread_create(&FileReaderThreadID,NULL,FileReader, NULL) != 0) {
                perror("failed to create a File Reader Thread!");
                        exit(-1);
                                                                    }
}

void *FileReader(void *threadid)
{
int fd;

fprintf(stdout, "FileReader Thread started. Reading xml data from %s\n", DATAfilename);
fprintf(logfile, "FileReader Thread started. Reading xml data from %s\n", DATAfilename);

// detach the thread so the resources may be returned when the thread exits
        pthread_detach(pthread_self());

        while (terminateFlag == 0) {//outer loop starts           
if ((fd = open (DATAfilename, O_RDONLY, S_IRUSR)) < 0)
{
perror("open");
exit(-1);
} else break;
                                   }
        
        while (terminateFlag == 0) {//outer loop starts        
                readDataFile(fd);
                                   }
	close(fd);
        pthread_exit( (void *) 1 ); 
}

int readDataFile(int fd) 
{
char buf[512*MAX_LINE];
char buff[512*MAX_LINE];
int r, i, j;
char filter[8];
char refilter[9];
int start_len_next;
int msg_len;
size_t xlen;
xmlDoc *doc = NULL;
char xml_start [] = "\n<XML_MESSAGE length=\"";
char xml_start0 [] = "<XML_MESSAGE length=\"";
char xml_start1 [] = "\r<XML_MESSAGE length=\"";

memset(filter, '\0', sizeof(filter));
memset(refilter, '\0', sizeof(refilter));
memset(buff, '\0', sizeof(buff));
memset(buf, '\0', sizeof(buf));

/*
if ((r = read(fd, buff, strlen(start_next))) < 0)
{
perror("read");
exit(1);
}
*/

r = read(fd, buff, strlen( (char *) start_next)); 
if (r < 0)
{
perror("read");
return (-1);
}

//check for valid input
if (strncmp(xml_start, buff, strlen(xml_start)) == 0 || strncmp(xml_start0, buff, strlen(xml_start0)) == 0 || strncmp(xml_start1, buff, strlen(xml_start1)) == 0) ;
else return -1;

strcpy(buf, buff);
start_len_next = strlen(buff);

//extract the real length
// for (i=21, j=0; i<start_len_next, j < 8; i++, j++) {  /* Wed May 21 11:16:27 PDT 2014 */

/* avoid left-hand operand of comma expression has no effect [-Wunused-value] in gcc 4.8+ */
for (i=21, j=0; j < 8; i++, j++) { /* Wed May 21 11:16:27 PDT 2014 */


        filter[j] = buf[i];
                                                   }
strncpy(refilter, filter, 7);
refilter[7] = filter[7];
refilter[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

msg_len = atoi(refilter);

strcpy(buff, "");
r = read(fd, buff, msg_len - start_len_next);
strncat(buf, buff, msg_len - start_len_next); 

xlen = xmlStrlen((void *)buf);

if (xlen > 8) //if less then that it is probably some junk...
{
        doc = xmlParseMemory(buf, sizeof(buf));
        if (doc == NULL)
            ;
        else
            //writeQueue(XMLQ, (void *)buf);
            //writeQueues((void *) buf);
            writeQueueTable((void *) buf);
        
        xmlFreeDoc(doc);
        doc = NULL; /*Fri Jun  6 15:27:46 MDT 2014 */
}

//printf("sendFile: buf chunk is:%s\n", buf);

strcpy(buf, "");
memset(buff, '\0', sizeof(buff));
memset(buf, '\0', sizeof(buff));
/*
if ((r = read(fd, buff, strlen("\n"))) < 0);//advance to a new message in a file
{
perror("read");
exit(1);
}
*/
r = read(fd, buff, strlen("\n"));//advance to a new message in a file
if (r < 0)
{
perror("read");
return (-1);
}
        return 0;
}

void *Reader(void *threadid)
{
int lsock, sock;
lsock = (long) threadid;
u_char *xmlR = NULL;
char ipstring[256];
struct sockaddr_in clientName = { 0 }; 
socklen_t clientNameLen = sizeof(clientName);

int myID;
Queue XMLQ = NULL;

int i,j;
char filter[8];
char refilter[9];
int msg_len;

// detach the thread so the resources may be returned when the thread exits
        pthread_detach(pthread_self());

myID = get_threadIDCounter(); /* obtain the ID of the queue to read from by brute force */
update_threadIDCounter();
XMLQ = QT->qtable[myID];
//XMLQ = XMLQS[myID];        


memset(ipstring, '\0', sizeof(ipstring));
memset(filter, '\0', sizeof(filter));
memset(refilter, '\0', sizeof(refilter));

struct timespec sleeptime;
sleeptime.tv_sec = 0;
sleeptime.tv_nsec = 1000;

        while (terminateFlag == 0) {//outer loop starts    
//if ((sock = accept(lsock, (struct sockaddr *)&server_sin, &len)) < 0 )
//if ((sock = accept(lsock, NULL, NULL)) < 0 ) {//might be a better alternative - gives less cases of detach failures
if ((sock = accept(lsock, (struct sockaddr *)&clientName, &clientNameLen)) < 0 ) { //need this version to get info about subscriber's IP
        perror("Reader: can't accept on a socket!");
	close(sock);
        continue;
        } else {
getPeerAddress ((struct sockaddr *) &clientName, ipstring);
        fprintf(stdout, "Reader: Accepting incoming client from IP: %s\n", ipstring);
//updateClientsTable(XMLQ, ipstring);
updateQueueTableClientsTable(QT, ipstring, 1);

//write introductory bgp message with <xml> tag to order the reading correctly in the client
write(sock, client_intro, strlen((char *)client_intro));
	       }
	//inner processing loop
	while (true) {
if (isQueueEmpty(XMLQ) == true) {
	continue;
	} else {
//do a random sleep for some time
nanosleep(&sleeptime, NULL);
readQueue( XMLQ, (void **)&xmlR);

if (xmlR != NULL)
; else continue;

size_t xlen = xmlStrlen((void *)xmlR);

	if (xlen > 2) //if xmlR is an actual full BGP message........
	{
//extract the real length
// for (i=21, j=0; i<xlen, j < 8; i++, j++) {  /* Wed May 21 11:16:27 PDT 2014 */

/* avoid left-hand operand of comma expression has no effect [-Wunused-value] in gcc 4.8+ */
for (i=21, j=0; j < 8; i++, j++) {  /* Wed May 21 11:16:27 PDT 2014 */
            
        filter[j] = xmlR[i];
                                         }
strncpy(refilter, filter, 7);
refilter[7] = filter[7];
refilter[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

msg_len = atoi(refilter);
fprintf(logfile, "Reader: sending the full XML entry from the Queue\n" );

//if (write(sock, xmlR, msg_len) != msg_len)//we send just the exact message length to avoid junk after that
//if (send(sock, xmlR, msg_len, 0) != msg_len)////we send just the exact message length to avoid junk after that
if ((write(sock, xmlR, msg_len) == -1)) {
fprintf(logfile, "Reader: client closed the connection\n" );
memset(refilter, '\0', sizeof(refilter));
xmlR = NULL;
		break;//break out of the inner while loop
					 } else {
memset(refilter, '\0', sizeof(refilter));
xmlR = NULL;
						}
	} else {//if xmlR is NOT an actual full BGP message........
memset(refilter, '\0', sizeof(refilter));
xmlR = NULL;
	       }

	       }//end of else when queue is not empty
		     }//end of inner while loop
        fprintf(logfile, "Reader: exited the inner while loop!\n");
                updateQueueTableClientsTable(QT, ipstring, 2);
		close(sock);
		}//end of outer while loop
                
                //update_terminateCounter();
                
                pthread_exit( (void *) 1 );
}//end of Reader

void initialize_terminateLock(void)
{
        if (pthread_mutex_init( &terminateLock, NULL ) )
		{
                perror( "unable to init mutex lock for queue");
		exit(1);
		}
}
        
int update_terminateCounter(void)
{
    // int swap = 0; /* intermediary variable Wed May 21 11:16:27 PDT 2014 */
    
        if ( pthread_mutex_lock( &terminateLock ) )
		{
                perror( "lockCounter: failed");
		exit(1);	
		}
    
        // terminateCounter = terminateCounter++; /* Wed May 21 11:16:27 PDT 2014 */
    
        /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
        // swap = terminateCounter++; /* Wed May 21 11:16:27 PDT 2014 */
        // terminateCounter = swap; /* Wed May 21 11:16:27 PDT 2014 */
        
        terminateCounter++; //JUST update the counter /* Thu May 22 15:36:29 PDT 2014 */
        
         if ( pthread_mutex_unlock( &terminateLock ) )
		{
                perror( "unlockCounter: failed");
		exit(1);	
		}
       return terminateCounter;
}

int get_terminateCounter(void)
{
    int t = 0;
     
        if ( pthread_mutex_lock( &terminateLock ) )
		{
                perror( "lockCounter: failed");
		exit(1);	
		}
    
       t = terminateCounter;
        
         if ( pthread_mutex_unlock( &terminateLock ) )
		{
                perror( "unlockCounter: failed");
		exit(1);	
		}
       return t;
}

void initialize_threadIDLock(void)
{
        if (pthread_mutex_init( &threadIDLock, NULL ) )
		{
                perror( "unable to init mutex ID lock");
		exit(1);
		}
}
        
int update_threadIDCounter(void)
{
    // int swap = 0; /* intermediary variable Wed May 21 11:16:27 PDT 2014 */
    
        if ( pthread_mutex_lock( &threadIDLock ) )
		{
                perror( "lockCounter: failed");
		exit(1);	
		}
    
       // threadIDCounter = threadIDCounter++; /* Wed May 21 11:16:27 PDT 2014 */
       
       /* use swap variable to avoid undefined operation  Wed May 21 11:16:27 PDT 2014 */
       // swap = threadIDCounter++; /* Wed May 21 11:16:27 PDT 2014 */
       // threadIDCounter = swap; /* Wed May 21 11:16:27 PDT 2014 */
        
       threadIDCounter++;  //JUST update the counter /* Thu May 22 15:36:29 PDT 2014 */
        
         if ( pthread_mutex_unlock( &threadIDLock ) )
		{
                perror( "unlockCounter: failed");
		exit(1);	
		}
       return threadIDCounter;
}

int get_threadIDCounter(void)
{
    int t = 0;
     
        if ( pthread_mutex_lock( &threadIDLock ) )
		{
                perror( "lockCounter: failed");
		exit(1);	
		}
    
       t = threadIDCounter;
        
         if ( pthread_mutex_unlock( &threadIDLock ) )
		{
                perror( "unlockCounter: failed");
		exit(1);	
		}
       return t;
}

int analyze (char *filename, xmlNode *root_element)
{
int exp_n = 0;
int cycle_and[256];
int cycle_or[256];
bool oracle_and = true;
bool oracle_or = false;
bool oracle = false;

bool and_found = false;

int and_count = 0; 
int or_count = 0;

int i;
int fd;

//case when complex = true
if (complex == true) {
//section to use after receiving xml chunk

        for (i=0; i < exp_gn; i++) {
                if (plogic[i] == '&')
		and_found = true;	
				   }

        for (i=0; i < exp_gn; i++) {
        if (harvest(expg[i], root_element) == 0)
        truth[i] = 1;
                                   }

        for (i=0; i < exp_gn; i++)
        fprintf(logfile, "truth is:%d\n", truth[i]);

        for (i=0; i < exp_gn; i++) {
        if (plogic[i] == '&' && truth[i] == 1)
                        ;
                        else {
                if (plogic[i] == '&' && truth[i] != 1)
                        g_and = false;
                        break;
                             }
                                   }

        for (i=0; i < exp_gn; i++) {
        if (plogic[i] == '|' && truth[i] == 1) {
                        g_or = true;
                        break;
					       }
                                   }
if ((g_and == true && and_found == true) || (g_or == true)) {
fprintf(logfile, "analyze: complex exp - g_and is true\n");
fd = open (filename, O_CREAT | O_APPEND | O_RDWR, S_IRWU);
if (bgp_m_ptr != NULL)
write(fd, bgp_m_ptr, strlen(bgp_m));
if (strlen (bgp_m) != 0)
write(fd, bgp_m, strlen(bgp_m));
fprintf(logfile, "analyze: complex exp - g_and is true - just about to write into file\n");
if (data_ptr != NULL)
write(fd, data_ptr->out, strlen(data_ptr->out));
//put the separator per message
write(fd, "\n", strlen("\n"));
fprintf(logfile, "analyze: complex exp - g_and is true - wrote into file\n");
 
MatchingMessages++; //increase the matching count

//now we put the data in queue as well
if (serverflag == 1) {
if (data_ptr != NULL) {
int xlen = strlen(data_ptr->out);
	if (xlen > 64)  //if xmlR is an actual full BGP message........
	{
fprintf(logfile, "Writer: putting the message into the Queue\n" );
//writeQueues((void *)data_ptr->out);
writeQueueTable((void *) data_ptr->out);
	} else
fprintf(logfile, "Writer: not putting the message into the Queue\n" );
		      }
		}//end of server flag

g_and = true; //restore to default
g_or = false; //restore to default
//initialize the parsing engine globals to nulls
for (i=0; i < 256; i++)
        truth[i] = 0;

	close(fd);//properly close the file to avoid running out of FDs
	return 0;
		   					}//end of checking oracles
                else {
fprintf(logfile, "g_and or g_or is false\n");
//initialize the parsing engine globals to nulls
for (i=0; i < 256; i++)
        truth[i] = 0;
        return -1;
		     }
		     } //end of case: complex = true 

//General case when complex = false
//lets fill the logic arrays with zeroes initially
for (i=0; i < 256; i++){
	cycle_and[i] = 0;
	cycle_or[i] = 0;
		       }

//lets count the number of expressions
for (i=0; i < 0; i++);
while (logic[i] == '&' || logic[i] == '|'){
	i++; exp_n++;
					 }

if (exp_n == 0)
if (strlen(element[0]) != 0 && operator[0] != 0 && strlen(value[0]) != 0)
	exp_n = 1;

fprintf(logfile, "exp_n is:%d\n", exp_n);

//lets count the AND and OR
for (i=0; i < exp_n; i++) {
	if (logic[i] == '&')
	and_count++;
	if (logic[i] == '|')
	or_count++;
			  }


for (i=0; i < exp_n; i++)//loop through defined tags/attributes of the expression
		{
if (strcmp(element[i], "PREFIX") == 0) {//operate on specified network prefix
if (get_prefix(root_element, element[i], operator[i], value[i]) == 0){	
		match = -1; //restore the match to default
		if (logic[i] == '&') {
		cycle_and[i] = 1;
				     }
		if (logic[i] == '|') {
		cycle_or[i] = 1;
				     }
		if (exp_n == 1) oracle = true; //if there is just one exp
								     }
				       }	
		else if (strcmp(element[i], "length") == 0 || strcmp(element[i], "version") == 0 || strcmp(element[i], "type") == 0 || strcmp(element[i], "value") == 0 || strcmp(element[i], "code") == 0 || strcmp(element[i], "timestamp") == 0 || strcmp(element[i], "datetime") == 0 || strcmp(element[i], "precision_time") == 0 || strcmp(element[i], "withdrawn_len") == 0 || strcmp(element[i], "path_attr_len") == 0 || strcmp(element[i], "label") == 0) {//operate on specified attributes
if (get_attribute(root_element, element[i], operator[i], value[i]) == 0){	
		if (logic[i] == '&') {
		cycle_and[i] = 1;
				     }
		if (logic[i] == '|') {
		cycle_or[i] = 1;
				     }
		if (exp_n == 1) oracle = true; //if there is just one exp
								      	}
		       }
		else {
if (get_element(root_element, element[i], operator[i], value[i]) == 0){//operate on elements	
		if (logic[i] == '&') {
		cycle_and[i] = 1;
				     }
		if (logic[i] == '|') {
		cycle_or[i] = 1;
				     }
		if (exp_n == 1) oracle = true; //if there is just one exp
								      }
		     }
		}//end of for loop		

	fprintf(logfile, "count AND: %d\n", and_count);
	fprintf(logfile, "count OR: %d\n", or_count);

for (i=0; i < and_count; i++)
	fprintf(logfile, "cycle array AND: %d\n", cycle_and[i]);
//for (i=0; i < or_count; i++)
for (i=0; i < 256; i++)
	if (cycle_or[i] == 1)
	fprintf(logfile, "cycle array OR: %d\n", cycle_or[i]);

//for (i=0; i < and_count; i++)
for (i=0; i < exp_n; i++) {
	if (cycle_and[i] != 1) {
	oracle_and = false;
	break;
			       }
			      }
for (i=0; i < exp_n; i++) {
	if (cycle_or[i] == 1) {
	oracle_or = true;
	break;
			      }
			  }

if (oracle_and == true || oracle_or == true || oracle == true)
				{
fprintf(logfile, "analyze: simple exp - one of the oracles is true\n");
fd = open (filename, O_CREAT | O_APPEND | O_RDWR, S_IRWU);
if (bgp_m_ptr != NULL)
write(fd, bgp_m_ptr, strlen(bgp_m));
if (strlen (bgp_m) != 0)
write(fd, bgp_m, strlen(bgp_m));
if (data_ptr != NULL)
write(fd, data_ptr->out, strlen(data_ptr->out));
//put the separator per message
write(fd, "\n", strlen("\n"));

MatchingMessages++; //increase the matching count

//now we put the data in queue as well
if (serverflag == 1) {
if (data_ptr != NULL) {
int xlen = strlen(data_ptr->out);
	if (xlen > 64) //if xmlR is an actual full BGP message........
	{
fprintf(logfile, "Writer: putting the message into the Queue\n" );
//writeQueues((void *)data_ptr->out);
writeQueueTable((void *) data_ptr->out);
	} else
fprintf(logfile, "Writer: not putting the message into the Queue\n" );
		      }
		}//end of server flag 

				}//end of checking oracles
	close(fd);//properly close the file to avoid running out of FDs
	return 0;
}

//void *StatusThread(void *threadid)
//{
//int lsock, sock, i; 
//float ratio = 0;
//lsock = (long) threadid;
//char value[256];
//int clientcount;
//
//fprintf(stdout, "Status Thread started. Listening on port 50009\n");
//fprintf(logfile, "Status Thread started. Listening on port 50009\n");
//
//// detach the thread so the resources may be returned when the thread exits
//        pthread_detach(pthread_self());
//
//        while (terminateFlag == 0) {//outer loop starts      
////if ((sock = accept(lsock, (struct sockaddr *)&server_sin, &len)) < 0 )
//if ((sock = accept(lsock, NULL, NULL)) < 0 )
//	{
//        perror("Status Thread: can't accept on a socket!");
//	close(sock);
//        continue;
//        } else {
//clientcount = getRefcount(XMLQ);
//write(sock, (char *) "STATUS DATA START:\n", strlen((char *)"STATUS DATA START:\n"));
//                        if(clientcount > 0)
//                        {
//write(sock, (char *) "Number of connected clients is:\n", strlen((char *)"Number of connected clients is:\n"));
//sprintf(value, (char *) "%d", clientcount);
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//write(sock, "Client IPs are:\n", strlen((char *)"Client IPs are:\n"));
//                                for (i = 0; i < clientcount; i++)
//                                {
//                                    char ip[INET_ADDRSTRLEN];
//                                    memset(ip, '\0', sizeof(ip));
//                                    getClientAddress(XMLQ, i, ip);                                 
//					write(sock, ip, strlen((char *)ip));
//					write(sock, "\n", strlen((char *)"\n"));
//					memset(ip, '\0', sizeof(ip));
//                    
//                                }
//                                
//                        }
//
//long items = getItemsUsed(XMLQ);
//sprintf(value, (char *) "%ld", items);
//write(sock, "Current number of items in the Queue is:\n", strlen((char *)"Current number of items in the Queue is:\n"));
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//
//sprintf(value, (char *) "%d", HCD);
//write(sock, "HCD is:\n", strlen((char *)"HCD is:\n"));
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//
//sprintf(value, (char *) "%lld", TotalMessagesReceived);
//write(sock, "TotalMessagesReceived:\n", strlen((char *)"TotalMessagesReceived:\n"));
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//
//sprintf(value, (char *) "%lld", MatchingMessages);
//write(sock, "MatchingMessages:\n", strlen((char *)"MatchingMessages:\n"));
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//
//ratio = ((float) MatchingMessages / (float) TotalMessagesReceived) * 100;
//
//sprintf(value, (char *) "%f", ratio);
//write(sock, "Ratio %:\n", strlen((char *)"Ratio %:\n"));
//write(sock, value, strlen(value));
//write(sock, "\n", strlen((char *)"\n"));
//memset(value, '\0', sizeof(value));
//
//write(sock, (char *) "STATUS DATA END:\n", strlen((char *)"STATUS DATA END:\n"));
//		close(sock);
//	       }
//		      }//end of outer while loop
//        pthread_exit( (void *) 1 ); 
//}//end of StatusThread

/* simplified version since most of the functionality will be moved to cli */
void *StatusThread(void *threadid)
{
int lsock, sock; 
float ratio = 0;
lsock = (long) threadid;
char value[256];
int i, clientcount;

fprintf(stdout, "Status Thread started. Listening on port %d \n", STATUS_PORT);
fprintf(logfile, "Status Thread started. Listening on port %d \n", STATUS_PORT);

// detach the thread so the resources may be returned when the thread exits
        pthread_detach(pthread_self());

        while (terminateFlag == 0) {//outer loop starts      
//if ((sock = accept(lsock, (struct sockaddr *)&server_sin, &len)) < 0 )
if ((sock = accept(lsock, NULL, NULL)) < 0 )
	{
        perror("Status Thread: can't accept on a socket!");
	close(sock);
        continue;
        } else {

write(sock, (char *) "STATUS DATA START:\n", strlen((char *)"STATUS DATA START:\n"));

sprintf(value, (char *) "%lld", TotalMessagesReceived);
write(sock, "TotalMessagesReceived:\n", strlen((char *)"TotalMessagesReceived:\n"));
write(sock, value, strlen(value));
write(sock, "\n", strlen((char *)"\n"));
memset(value, '\0', sizeof(value));

sprintf(value, (char *) "%lld", MatchingMessages);
write(sock, "MatchingMessages:\n", strlen((char *)"MatchingMessages:\n"));
write(sock, value, strlen(value));
write(sock, "\n", strlen((char *)"\n"));
memset(value, '\0', sizeof(value));

ratio = ((float) MatchingMessages / (float) TotalMessagesReceived) * 100;

sprintf(value, (char *) "%f", ratio);
write(sock, "Ratio %:\n", strlen((char *)"Ratio %:\n"));
write(sock, value, strlen(value));
write(sock, "\n", strlen((char *)"\n"));
memset(value, '\0', sizeof(value));

clientcount = getQueueTableRefcount(QT);

if(clientcount > 0)
{

write(sock, (char *) "Number of connected clients is:\n", strlen((char *)"Number of connected clients is:\n"));
sprintf(value, (char *) "%d", clientcount);
write(sock, value, strlen(value));
write(sock, "\n", strlen((char *)"\n"));
memset(value, '\0', sizeof(value));

write(sock, "Client IPs are:\n", strlen((char *)"Client IPs are:\n"));
                                for (i = 0; i < clientcount; i++)
                                {
                                    char ip[INET_ADDRSTRLEN];
                                    memset(ip, '\0', sizeof(ip));
                                    getQueueTableClientAddress(QT, i, ip);                                 
					write(sock, ip, strlen((char *)ip));
					write(sock, "\n", strlen((char *)"\n"));
					memset(ip, '\0', sizeof(ip));
                    
                                }
}


write(sock, (char *) "STATUS DATA END:\n", strlen((char *)"STATUS DATA END:\n"));
		close(sock);
	       }
		      }//end of outer while loop
        pthread_exit( (void *) 1 ); 
}//end of StatusThread


void prompt(int s) 
{
        write(s, (char *) "$_:\0", strlen((char *)"$_:\0"));
}

/* pretty strange function */
int clear(char * a, char * b, size_t s)
{
        strcpy(a, "");
        strcpy(b, ""); 
        memset(a, '\0', s);
        memset(b, '\0', s);
        strcpy(a, "");
        strcpy(b, "");  
        return 0;
}

int resetExp(void)
{
        strcpy(expression, "");
        memset(expression, '\0', sizeof(expression));
        strcpy(expression, "");
        return 0;
}

int resetMatch(void)
{
        MatchingMessages = 0;
        return 0;
}

void *CLIThread(void *threadid)
{
int lsock, sock, result; 
lsock = (long) threadid;
char buffer[256];
char tmp[256];
char exp[4096];
char tmp0[4096];

fprintf(stdout, "CLI Thread started. Listening on port %d \n", CLI_PORT);
fprintf(logfile, "CLI Thread started. Listening on port %d \n", CLI_PORT);

// detach the thread so the resources may be returned when the thread exits
        pthread_detach(pthread_self());

        clear(tmp, buffer, sizeof(buffer));
        
        while (terminateFlag == 0) {//outer loop starts       
//if ((sock = accept(lsock, (struct sockaddr *)&server_sin, &len)) < 0 )
if ((sock = accept(lsock, NULL, NULL)) < 0 )
	{
        perror("CLI Thread: can't accept on a socket!");
	close(sock);
        continue;
        } else {
    
        clear(tmp, buffer, sizeof(buffer));
        clear(tmp0, exp, sizeof(exp));

        write(sock, (char *) "Enter password:\n", strlen((char *)"Enter password:\n"));
        
if ((result = recv(sock, tmp, sizeof(buffer), 0)) > 2)
        strncpy (buffer, tmp, result-2);
else continue;

if (strcmp(DEFAULT_ACCESS_PASSWORD, buffer) == 0)
{
    for (; ;) {
        
    clear(tmp, buffer, sizeof(buffer));
    prompt(sock);
    
    if ((result = recv(sock, tmp, sizeof(buffer), 0)) < 2)
        break;
    else strncpy (buffer, tmp, result-2);
    if (strcmp(buffer, RT_STRING) == 0)
        continue;
if (strcmp(buffer, EXIT_STRING) == 0 || strcmp(buffer, EXIT_STR) == 0 || strcmp(buffer, "q") == 0)
        break;
else if (strcmp(buffer, HELP_STR) == 0 || strcmp(buffer, "h") == 0)
        {
        write(sock, (char *) "available commands are :\n", strlen((char *)"available commands are :\n"));
        write(sock, (char *) "help (h); exit (q); show transaction (st); change transaction (ct); reset transaction (rt); shutdown (sd)\n", strlen((char *)"help (h); exit (q); show transaction (st); change transaction (ct); reset transaction (rt); shutdown (sd)\n"));
        }        
else if (strcmp(buffer, SHUTDWN_STR) == 0 || strcmp(buffer, "sd") == 0)
        //terminate_(0);
        setTerminateFlag(0);
else if (strcmp(buffer, RESET_TRANSACTION_STR) == 0 || strcmp(buffer, "rt") == 0)
        resetExp();
else if (strcmp(buffer, SHOW_TRANSACTION_STR) == 0 || strcmp(buffer, "st") == 0)
        {
        write(sock, (char *) expression, strlen((char *) expression));
        write(sock, (char *) "\n", strlen((char *) "\n"));
        }
else if (strcmp(buffer, TRANSACTION_STR) == 0 || strcmp(buffer, "ct") == 0)
        {
        write(sock, (char *) "enter new transaction:\n", strlen((char *)"enter new transaction:\n"));
        clear(tmp0, exp, sizeof(exp));
        if ((result = recv(sock, tmp0, sizeof(tmp0), 0)) < 2)
                continue;
        else {
             strncpy (exp, tmp0, result-2);
             if (strlen(exp) <= 4) continue; //we assume the expression smaller then 4 characters is a fake one
             else ReInitializeParseEngine(eflag, exp);
             }
        }
else {
     write(sock, (char *) "unknown command. type h for help\n", strlen((char *)"unknown command. type h for help\n"));
     }    
              }//end of for loop
              close(sock);//terminate connection immediately upon exit
}    else close(sock); //end of passwd checking - if passwd does not match 
//printf("buffer len is: %d ", strlen(buffer));
	       }//end of else for accept()
		      }//end of outer while loop
        pthread_exit( (void *) 1 ); 
}//end of CLI Thread

void terminate_(int sigtype)
{
sigset_t OldMask;
sigprocmask(SIG_BLOCK, &term_handler.sa_mask, &OldMask);
setTerminateFlag(0);
usleep(100000);
	
if (serverflag == 1)
	{
            //NO NEED for this anymore since each thread is checking the TerminateFlag now
//	if (pthread_cancel(StatusThreadID) != 0)
//                perror("Status Thread cancellation failed!");
//        
//        if (pthread_cancel(CLIThreadID) != 0)
//                perror("CLI Thread cancellation failed!");
//        
//        if (pthread_cancel(FileWriterThreadID) != 0)
//                perror("CLI Thread cancellation failed!");
//
//        //cancel the client serving thread pool
//        for(tindex = 0; tindex < R_NUM; tindex++) {
//                if (pthread_cancel(tidR[tindex]) != 0) {
//                        perror("failed to cancel one of the client handling Reader threads!");
//			free(tidR);
//                        exit(-1);
//                                                       }
//                                                  }
            
            //while (get_terminateCounter() < R_NUM); //wait till all Readers update the counter before they exit
            
                pthread_attr_destroy(&attr);                                  
                free(tidR); //free the client serving thread pool
                removeQueueTable();
        }
        
    	xmlCleanupParser(); //Free the global libxml variables 
        resetXmlBuff();
	
	sigemptyset(&term_handler.sa_mask);
	memset(&term_handler, 0, sizeof(term_handler));
	sigprocmask(SIG_SETMASK, &OldMask, NULL);
        
        fprintf(stdout, "TeleScope received TERM signal. Freed all memory, canceled all threads and now exiting gracefully.\n");
        fprintf(logfile, "TeleScope received TERM signal. Freed all memory, canceled all threads and now exiting gracefully.\n");
        fclose (logfile);
	exit (0);
}

int resetXmlBuff(void)
{
        if (data_ptr == NULL) { data_ptr = NULL; return -1; } //KB: have to check memory allocation
        
        if (data_ptr != NULL)
	{
	memset(data_ptr->in, '\0', sizeof(data_ptr->in));
	memset(data_ptr->out, '\0', sizeof(data_ptr->out));
	data_ptr =  realloc(data_ptr, sizeof(char));
        if (data_ptr != NULL)
                free(data_ptr);
	data_ptr = NULL;
	}
        return 0;
}

void processStream(void)
{
 while (terminateFlag != 1) {

data_ptr = malloc(sizeof(data));

if (data_ptr == NULL) { data_ptr = NULL; return; } //KB: have to check memory allocation

strcpy(data_ptr->in, "");
strcpy(data_ptr->out, "");

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
memset(data_ptr->in, '\0', sizeof(data_ptr->in));
memset(data_ptr->out, '\0', sizeof(data_ptr->out));
data_ptr =  realloc(data_ptr, sizeof(char));
if (data_ptr != NULL)
        free(data_ptr);
data_ptr = NULL;

          		   }//end of main loop   
}

int reset_key(void)
{
memset(heItem.key, 0, sizeof(heItem.key));
return 0;
}



void print_service_ports(void) /* Mon May 26 17:15:55 MDT 2014 */ 
{
    fprintf(stdout, "When in server mode TeleScope is listening on the following ports: \n");
    fprintf(stdout, "Clients Thread Pool - port %d \n", SERVER_PORT);
    fprintf(stdout, "Status Thread - port %d \n", STATUS_PORT);
    fprintf(stdout, "CLI Thread - port %d \n", CLI_PORT);
}

int godaemon (char *rundir, char *pidfile, int noclose)
{
	char str[10];
	int pidFilehandle;
	pid_t pid;

	/* Check if parent process id is set */
	if (getppid() == 1)
	{
		fprintf(stderr,"PPID exists, therefore we are already a daemon ");
		return -1;
	}

	/* Fork*/
	pid = fork ();

	/* In case of fork is error. */
	if (pid < 0)
	{
		fprintf (stdout, "fork failed: %s", strerror(errno));
		return -1;
	}

	/* In case of this is parent process. */
	if (pid != 0)
		exit (0);

	/* Become session leader and get pid. */
	pid = setsid();

	if (pid == -1)
	{
		fprintf (stdout, "setsid failed: %s", strerror(errno));
		return -1;
	}

	/* Change directory to root. */
	//  if (! nochdir)
	//    chdir ("/");

	/* File descriptor close. */
	if (! noclose)
	{
		int fd;
		fd = open ("/dev/null", O_RDWR, 0);
		if (fd != -1)
		{
			dup2 (fd, STDIN_FILENO);
			dup2 (fd, STDOUT_FILENO);
			dup2 (fd, STDERR_FILENO);
			if (fd > 2)
				close (fd);
		}
	}

	umask (027);

	chdir(rundir); /* change running directory */

        /* Ensure only one copy */
        pidFilehandle = open(pidfile, O_RDWR|O_CREAT, 0600);
        if (pidFilehandle == -1 )
        {
                /* Couldn't open lock file */
                fprintf (stdout, "Could not open PID lock file: %s", pidfile);
                return -1;
       }

        /* Try to lock file */
        if (lockf(pidFilehandle,F_TLOCK,0) == -1)
        {
            /* Couldn't get lock on lock file */
            fprintf (stdout, "Could not lock PID lock file %s", pidfile);
            return -1;
        }

        /* Get and format PID */
        sprintf(str,"%d\n",getpid());

        /* write pid to lockfile */
        write(pidFilehandle, str, strlen(str));


	return 0;
}

        
#endif	/* FUNCTIONS_H */

