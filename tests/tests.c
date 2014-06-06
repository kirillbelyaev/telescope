/*
 * File:   newsimpletest.c
 * Author: kirill
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




int main(int argc, char** argv) {
    
    //char evalue [] = "(type = STATUS) | (type = UPDATE)"; 
    char evalue [] = "(type = STATUS)"; 
    
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
    
    processStream(); //passed test both with 0/-1 input from receive_xml()
    
    
    
    
    return (EXIT_SUCCESS);
}
