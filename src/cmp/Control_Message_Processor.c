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
* File: Control_Message_Processor.C
* ----------------------------------
* For parsing an XML control message, filtering out the Upstream Brokers List and Filtering Query 
* from the message and performing validation using Secure Hash Algorithm 1.
*/

#include "Control_Message_Processor.h"

#define CMP_MAX_UBL 100000

char *Control_Message_Processor_CONT = NULL;
uint_fast8_t Control_Message_Processor_Flag = 0;
char *Control_Message_Processor_UBL = NULL;
char *Control_Message_Processor_FQ = NULL;
char *Control_Message_Processor_SHA1 = NULL;

uint_fast8_t Control_Message_Processor_CRL_MESSAGE_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_CONTROLMESSAGELENGTH_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_CONTROLMESSAGEID_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_ROOTBROKERID_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_CHILDBROKERID_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_UBL_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_SHA1_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_FQ_ELEMENT = 0;
uint_fast8_t Control_Message_Processor_ROOT_ELEMENT_VALID = 0;
uint_fast8_t Control_Message_Processor_ISVALID = 0;
uint_fast8_t Control_Message_Processor_DEPTH = 0;
uint_fast8_t Control_Message_Processor_CONTENT_FLAG = 0;
uint_fast8_t Control_Message_Processor_ELEMENT_CASE = 0;
uint_fast8_t Control_Message_Processor_ATTRIBUTE_CASE = 0;

unsigned long int Control_Message_Processor_MESSAGE_LENGTH = 0;
uint_fast8_t Control_Message_Processor_LENGTH_VALID = 0;

void delay(unsigned int seconds)
{
	int elapsed = time(0) + seconds;
	while (time(0) < elapsed);
}

int isUpperCase(char* str)
{
	int flag = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] >= 'A' && str[i] <= 'Z')
		{
			;
		}
		else
		{
			flag = 1;
		}
	}
	if (flag == 1)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

// Expat handler for detecting element start tags.
void start(void *data, const char *element, const char **Attribute)
{
	Control_Message_Processor_DEPTH++;

	if (Control_Message_Processor_DEPTH == 1)
	{
		if (strcmp(element, "CRL_MESSAGE") == 0)
		{
			Control_Message_Processor_ROOT_ELEMENT_VALID = 1;
		}
	}

	if (isUpperCase == 0)
	{
		Control_Message_Processor_ELEMENT_CASE = 1;
	}

	if (strcmp(element, "CRL_MESSAGE") == 0)
	{
		Control_Message_Processor_CRL_MESSAGE_ELEMENT = 1;
	}
	if (strcmp(element, "CONTROL_MESSAGE_LENGTH") == 0)
	{
		Control_Message_Processor_CONTROLMESSAGELENGTH_ELEMENT = 1;
	}
	if (strcmp(element, "CONTROL_MESSAGE_ID") == 0)
	{
		Control_Message_Processor_CONTROLMESSAGEID_ELEMENT = 1;
	}
	if (strcmp(element, "ROOT_BROKER_ID") == 0)
	{
		Control_Message_Processor_ROOTBROKERID_ELEMENT = 1;
	}
	if (strcmp(element, "CHILD_BROKER_ID") == 0)
	{
		Control_Message_Processor_CHILDBROKERID_ELEMENT = 1;
	}
	if (strcmp(element, "UBL") == 0)
	{
		Control_Message_Processor_UBL_ELEMENT = 1;
	}
	if (strcmp(element, "SHA1") == 0)
	{
		Control_Message_Processor_SHA1_ELEMENT = 1;
	}
	if (strcmp(element, "FILTERING_QUERY") == 0)
	{
		Control_Message_Processor_FQ_ELEMENT = 1;
	}

	if (Control_Message_Processor_Flag == 1)
	{
		strcat(Control_Message_Processor_UBL, element);
		strcat(Control_Message_Processor_UBL, ":");

		if (Attribute[0] != NULL)
		{
			if (strcmp(element, "CRL_MESSAGE") == 0)
			{
				if (strcmp(Attribute[0], "Length") == 0)
				{
					if (strcmp(atoi(Attribute[1]), Control_Message_Processor_MESSAGE_LENGTH) != 0)
					{
						Control_Message_Processor_LENGTH_VALID = 1;
					}
				}
			}
			uint_fast16_t i;
			for (i = 0; Attribute[i]; i += 2)
			{
				strcat(Control_Message_Processor_UBL, Attribute[i]);
				strcat(Control_Message_Processor_UBL, "=");
				strcat(Control_Message_Processor_UBL, Attribute[i + 1]);
				strcat(Control_Message_Processor_UBL, ",");
			}
		}
	}
	if (strcmp(element, "UBL") == 0)
	{
		Control_Message_Processor_Flag = 1;
	}
	if (strcmp(element, "FILTERING_QUERY") == 0)
	{
		Control_Message_Processor_Flag = 2;
	}
	if (strcmp(element, "SHA1") == 0)
	{
		Control_Message_Processor_Flag = 3;
	}
	if (strcmp(element, "CONTROL_MESSAGE_LENGTH") == 0)
	{
		Control_Message_Processor_Flag = 4;
	}
}

// Expat handler for detecting element end tags.
void end(void* data, const char* element)
{
	Control_Message_Processor_DEPTH--;

	// For Control_Message_Processor_Length
	if (Control_Message_Processor_Flag == 4)
	{
		if (strcmp(element, "CONTROL_MESSAGE_LENGTH") == 0)
		{
			Control_Message_Processor_Flag = 0;
		}
		
		if (Control_Message_Processor_MESSAGE_LENGTH != atoi(Control_Message_Processor_CONT))
		{
			Control_Message_Processor_LENGTH_VALID = 1;
		}
	}

	// For UBL
	if (Control_Message_Processor_Flag == 1)
	{
		if (strcmp(element, "UBL") == 0)
		{
			Control_Message_Processor_Flag = 0;
		}

		if (strstr(element, "BROKER") != NULL)
		{
			strcat(Control_Message_Processor_UBL, "\n");
		}
	}

	// For FQ.
	if (Control_Message_Processor_Flag == 2)
	{
		Control_Message_Processor_FQ = NULL;
		Control_Message_Processor_FQ = (char*)malloc(sizeof(char) * (strlen(Control_Message_Processor_CONT) + 1));
		if (Control_Message_Processor_FQ == NULL)
		{
			perror("\nError: Could not allocate memory for Filtering Query buffer. \n Terminating program...");
			delay(1);
			exit(0);
		}
		*Control_Message_Processor_FQ = '\0';
		strcat(Control_Message_Processor_FQ, Control_Message_Processor_CONT);
		if (strcmp(element, "FILTERING_QUERY") == 0)
		{
			Control_Message_Processor_Flag = 0;
		}
	}

	// For SHA-1.
	if (Control_Message_Processor_Flag == 3)
	{
		Control_Message_Processor_SHA1 = NULL;
		Control_Message_Processor_SHA1 = (char*)malloc(sizeof(char) * (strlen(Control_Message_Processor_CONT) + 1));
		if (Control_Message_Processor_SHA1 == NULL)
		{
			perror("\nError: Could not allocate memory for SHA-1 digest buffer. \n Terminating program...");
			delay(1);
			exit(0);
		}
		*Control_Message_Processor_SHA1 = '\0';
		strcat(Control_Message_Processor_SHA1, Control_Message_Processor_CONT);
		
		if (strcmp(element, "SHA1") == 0)
		{
			Control_Message_Processor_Flag = 0;
		}

	}
	if (strcmp(element, "CONTROL_MESSAGE_LENGTH") == 0 || strcmp(element, "ROOT_BROKER_ID") == 0 || strcmp(element, "CHILD_BROKER_ID") == 0 || strcmp(element, "SHA1") == 0)
	{
		if (strcmp(Control_Message_Processor_CONT, "") == 0)
		{
			Control_Message_Processor_CONTENT_FLAG = 1;
		}
	}
	strcpy(Control_Message_Processor_CONT, "");
}

// Expat handler for getting contents.
void content(void *data, const char *content, int length)
{
	char *temp = NULL;
	temp = (char*)malloc(sizeof(char*) * length);
	if (temp == NULL)
	{
		perror("\nError: Could not allocate memory for content buffer. \n Terminating program...");
		delay(1);
		exit(0);
	}
	strncpy(temp, content, length);
	temp[length] = '\0';
	data = (void*)temp;
	Control_Message_Processor_CONT = temp;
}

char* get_UBL()
{
	if (Control_Message_Processor_UBL == NULL)
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Memory not allocated. Incorrect usage of get_UBL.\n");
		delay(1);
		exit(0);
	}
	if (Control_Message_Processor_ISVALID == 1)
	{
		return Control_Message_Processor_UBL;
	}
	else
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Invalid message.\n");
		return NULL;
	}
}

char* get_FQ()
{
	if (Control_Message_Processor_FQ == NULL)
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Memory not allocated. Incorrect usage of get_FQ.\n");
		delay(1);
		exit(0);
	}
	if (Control_Message_Processor_ISVALID == 1)
	{
		return Control_Message_Processor_FQ;
	}
	else
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Invalid message.\n");
		return NULL;
	}
}

char* shafunc(char *buff)
{
	/*
	* 1. Create copy of buff in testbuf.
	* 2. Process testbuf to strip the existing <SHA1> element contents.
	* 3. Store this processed testbuf in testbuf2.
	* 4. Use testbuf2 to compute digest and return it.
	*/

	uint_fast16_t Buffer_Length = strlen(buff) + 1;

	char *testbuf = NULL;
	testbuf = (char*)malloc(sizeof(char*) * Buffer_Length);
	if (testbuf == NULL)
	{
		perror("\nError: Could not allocate memory for testbuf. \n Terminating program...");
		delay(1);
		exit(0);
	}
	strcpy(testbuf, buff);

	uint_fast8_t shaflag = 0;
	uint_fast16_t kc = 0;

	char *testbuf2 = NULL;
	testbuf2 = (char*)malloc(sizeof(char*) * Buffer_Length);
	if (testbuf2 == NULL)
	{
		perror("\nError: Could not allocate memory for testbuf2. \n Terminating program...");
		delay(1);
		exit(0);
	}

	char shatag[7];
	for (uint_fast16_t l = 0; testbuf[l] != '\0'; l++)
	{
		for (uint_fast16_t k = 0; k < 5; k++)
		{
			shatag[k] = shatag[k + 1];
		}
		shatag[5] = testbuf[l];
		shatag[6] = '\0';
		if (strcmp(shatag, "<SHA1>") == 0)
		{
			testbuf2[kc] = testbuf[l];
			kc++;
			shaflag = 1;
		}

		if (strcmp(shatag, "</SHA1") == 0)
		{
			testbuf2[kc] = '<';
			kc++;
			testbuf2[kc] = '/';
			kc++;
			testbuf2[kc] = 'S';
			kc++;
			testbuf2[kc] = 'H';
			kc++;
			testbuf2[kc] = 'A';
			kc++;
			shaflag = 0;
		}
		if (shaflag == 0)
		{
			testbuf2[kc] = testbuf[l];
			kc++;
		}
	}
	testbuf2[kc] = '\0';
	if (testbuf != NULL)
	{
		free(testbuf);
		testbuf = NULL;
	}

	const char* testhash = (const char*)testbuf2;
	unsigned char *hash = NULL;
	hash = (unsigned char*)malloc(sizeof(char*) * (SHA_DIGEST_LENGTH * 2 + 1));
	if (hash == NULL)
	{
		perror("\nError: Could not allocate memory for hash. \n Terminating program...");
		delay(1);
		exit(0);
	}

	SHA_CTX context;
	SHA1_Init(&context);
	SHA1_Update(&context, testhash, strlen(testhash));
	SHA1_Final(hash, &context);

	char *digest = NULL;
	digest = (char*)malloc(sizeof(char*)*(SHA_DIGEST_LENGTH * 2 + 1));
	if (digest == NULL)
	{
		perror("\nError: Could not allocate memory for digest. \n Terminating program...");
		delay(1);
		exit(0);
	}
	for (uint_fast8_t i = 0; i < SHA_DIGEST_LENGTH; i++)
		sprintf(&digest[i * 2], "%02x", (unsigned int)hash[i]);

	if (testbuf2 != NULL)
	{
		free(testbuf2);
		testbuf2 = NULL;
	}
	if (hash != NULL)
	{
		free(hash);
		hash = NULL;
	}

	return digest;
}

int_fast8_t filter(char *buff)
{
	Control_Message_Processor_CRL_MESSAGE_ELEMENT = 0;
	Control_Message_Processor_CONTROLMESSAGELENGTH_ELEMENT = 0;
	Control_Message_Processor_CONTROLMESSAGEID_ELEMENT = 0;
	Control_Message_Processor_ROOTBROKERID_ELEMENT = 0;
	Control_Message_Processor_CHILDBROKERID_ELEMENT = 0;
	Control_Message_Processor_UBL_ELEMENT = 0;
	Control_Message_Processor_SHA1_ELEMENT = 0;
	Control_Message_Processor_FQ_ELEMENT = 0;
	Control_Message_Processor_ROOT_ELEMENT_VALID = 0;
	Control_Message_Processor_ISVALID = 0;
	Control_Message_Processor_ELEMENT_CASE = 0;
	Control_Message_Processor_CONTENT_FLAG = 0;
	Control_Message_Processor_Flag = 0;
	Control_Message_Processor_DEPTH = 0;

	Control_Message_Processor_UBL = NULL;
	Control_Message_Processor_UBL = (char*)malloc(sizeof(char*) * CMP_MAX_UBL);
	Control_Message_Processor_MESSAGE_LENGTH = strlen(buff);

	if (Control_Message_Processor_UBL == NULL)
	{
		perror("\nError: Could not allocate memory for Upstream Brokers List buffer. \n Terminating program...");
		delay(1);
		exit(0);
	}
	*Control_Message_Processor_UBL = '\0';
	//printf("\nCMP length: %lu\n", Control_Message_Processor_MESSAGE_LENGTH);

	// Initiate parse.
	XML_Parser p = XML_ParserCreate(NULL);
	XML_SetElementHandler(p, start, end);
	XML_SetCharacterDataHandler(p, content);

	// Not well formed.
	if (XML_Parse(p, buff, strlen(buff), XML_TRUE) == XML_STATUS_ERROR)
	{
		return -1;
	}
	XML_ParserFree(p);

	// Root element.
	if (Control_Message_Processor_ROOT_ELEMENT_VALID == 0)
	{
		return -1;
	}

	// Missing elements.
	if (Control_Message_Processor_CRL_MESSAGE_ELEMENT == 0 || Control_Message_Processor_CONTROLMESSAGELENGTH_ELEMENT == 0 || Control_Message_Processor_CONTROLMESSAGEID_ELEMENT == 0 || Control_Message_Processor_ROOTBROKERID_ELEMENT == 0 || Control_Message_Processor_CHILDBROKERID_ELEMENT == 0 || Control_Message_Processor_UBL_ELEMENT == 0 || Control_Message_Processor_FQ_ELEMENT == 0 || Control_Message_Processor_SHA1_ELEMENT == 0)
	{
		return -1;	
	}

	// If any element (except UBL and FQ) is empty, return -1.
	if(Control_Message_Processor_CONTENT_FLAG == 1)
	{
		return -1;
	}

	// Element case check. 
	if (Control_Message_Processor_ELEMENT_CASE == 1)
	{
		return -1;
	}
	
	// Message length check.
	if (Control_Message_Processor_LENGTH_VALID == 1)
	{
		return -1;
	}

	// Check SHA-1 digest.
	char* digest = shafunc(buff);
	if (strcmp(Control_Message_Processor_SHA1, digest) == 0)
	{
		if (digest != NULL)
		{
			free(digest);
			digest = NULL;
		}
		Control_Message_Processor_ISVALID = 1;
		return 0;
	}
	else
	{
		return -1;
	}
}

void printUBL()
{
	if (Control_Message_Processor_UBL == NULL)
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Memory not allocated. Incorrect usage of printUBL.\n");
		delay(1);
		exit(0);
	}
	if (Control_Message_Processor_ISVALID == 1)
	{
		printf("\n");
		for (int i = 0; Control_Message_Processor_UBL[i] != '\0'; i++)
		{
			if (Control_Message_Processor_UBL[i] == '\n')
			{
				printf("\n");
			}
			else
			{
				printf("%c", Control_Message_Processor_UBL[i]);
			}
		}
	}
	else
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: Invalid message.\n");
		return NULL;
	}
}

void freeMem()
{
	if (Control_Message_Processor_SHA1 != NULL && Control_Message_Processor_FQ != NULL && Control_Message_Processor_UBL != NULL && Control_Message_Processor_CONT != NULL)
	{
		free(Control_Message_Processor_SHA1);
		Control_Message_Processor_SHA1 = NULL;

		free(Control_Message_Processor_FQ);
		Control_Message_Processor_FQ = NULL;

		free(Control_Message_Processor_UBL);
		Control_Message_Processor_UBL = NULL;

		free(Control_Message_Processor_CONT);
		Control_Message_Processor_CONT = NULL;
	}
	else
	{
		fprintf(stderr, "%s", "\nControlMessageProcessorError: A call to Control_Message_Processor.freeMem should always be preceded by a call to Control_Message_Processor.process.\n");
		delay(1);
		exit(0);
	}
}

int process(char *buff)
{
	int_fast8_t isValid;
	isValid	= filter(buff);
	if (isValid == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
