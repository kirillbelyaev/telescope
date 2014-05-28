/*
Copyright (c) 2012-2014  Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 * Hash Table Library support for TeleScope to enable Duplicate Suppression
*/

#ifndef HASHLIB_H
#define	HASHLIB_H

/* hash table cache functions */
/* hash table cache definitions */

#define HASHSIZE 4998
#define HASHTHLD HASHSIZE/8
char dict[1024][128];
int HCD; /* collision detection factor */
int slotIndex;//record how many slots in HT are used
typedef struct HashItem
{
        int used;
        char key[512]; /* key is the 3 tuple element used for hashing */
        int id;
        int seq_num;
        int timestamp;
        int datetime;
} HashEntry;

typedef HashEntry HashTable[HASHSIZE];

void createTable (HashTable HT);
void clearTable (HashTable HT);
int hash (char *k);
int insertHash (HashTable HT, HashEntry newitem);
int retrieveHash (HashTable HT, HashEntry target);
void printHashDistribution (HashTable HT);

int constructHash(xmlNode * a_node, HashEntry Item, HashTable ht);
//int get_id(xmlNode * a_node);
//xmlChar * get_id(xmlNode * a_node);
int get_id(xmlNode * a_node, xmlChar * buffer);
//xmlChar * get_seq_num(xmlNode * a_node);
int get_seq_num(xmlNode * a_node, xmlChar * buffer);
//xmlChar * get_timestamp(xmlNode * a_node);
int get_timestamp(xmlNode * a_node, xmlChar * buffer);
int get_datetime(xmlNode * a_node, xmlChar * buffer);

void random_init (void);
int new_rand (void);
int generateDict (void);

/* hash table cache  def end*/

void createTable (HashTable H)
{
HCD = 0;/* set hash collision detector factor to zero */
        int i; HashEntry Item;
        for (i = 0; i < HASHSIZE; i++) {
                H[i].used = 0; 
		memset(H[i].key, 0, sizeof(Item.key));
                H[i].id = -1; 
                H[i].seq_num = -1; 
                H[i].timestamp = -1; 
                H[i].datetime= -1; 
                                       }   
}

void clearTable (HashTable H)
{
        int i; HashEntry Item;
        for (i = 0; i < HASHSIZE; i++) {
                H[i].used = 0; 
		memset(H[i].key, 0, sizeof(Item.key));
                H[i].id = -1; 
                H[i].seq_num = -1; 
                H[i].timestamp = -1; 
                                       }   
HCD = 0;/* set hash collision detector factor to zero */
slotIndex = 0;
}

//simple hash function
int hash (char *s)
{
        unsigned h = 0;
        while (*s)
                h+=*s++;
        return h % HASHSIZE;
}

int insertHash (HashTable H, HashEntry newitem)
{
        int pc = 0; /* probe count to be sure that table is not full */
        int probe;  /* position currently probed in H */
        int increment = 1; /* increment used for quadratic probing */

        probe = hash(newitem.key);

        //while (H[probe].key != NULL /* is the location empty? */
        while (H[probe].used != 0 /* is the location empty? */
&& strcmp(newitem.key, H[probe].key) /* duplicate key present? */
 && pc <= HASHSIZE/2) /* has overflow occured? */
        {
                pc++;
                probe = (probe + increment) % HASHSIZE;
                increment += 2; /* prepare increment for next iteration */
        }/* end of while */

        //if (H[probe].key == NULL) { /* if the entry is not taken yet */
        if (H[probe].used == 0) { /* if the entry is not taken yet */
                H[probe].used = 1; /* insert the new entry */
                strcpy(H[probe].key, newitem.key); /* insert the new entry */
                H[probe].id = newitem.id; /* insert the new entry */
                H[probe].seq_num = newitem.seq_num; /* insert the new entry */
                H[probe].timestamp = newitem.timestamp; /* insert the new entry */
        fprintf(logfile, "Inserted entry with key %s into HT!\n", newitem.key);
	if (slotIndex < HASHSIZE)
		slotIndex++;//update the HT utilization
        } else if (strcmp(newitem.key, H[probe].key) == 0)
        {
        perror("duplicate entry in HT!\n");
        fprintf(logfile, "duplicate entry in HT!\n");
        fprintf(logfile, "element key to be inserted in HT is: %s\n", newitem.key);
        fprintf(logfile, "element key already inserted in HT is: %s\n", H[probe].key);
		fprintf(logfile, "HCD is: %d\n", HCD);
	if (HCD < HASHTHLD || slotIndex < HASHSIZE)
        HCD++; /* record the collision */
	else { printHashDistribution(H); clearTable(H); }
        return -1;
        }
        else
        {
        fprintf(logfile, "Hash table is full or collision occured !\n");
	if (HCD < HASHTHLD || slotIndex < HASHSIZE)
        HCD++; /* record the collision */
	else { printHashDistribution(H); clearTable(H); }
        return -1;
        }
			return 0;
}

int retrieveHash (HashTable H, HashEntry target)
{
        int pc = 0;
        int probe;
        int increment = 1;

        probe = hash(target.key);

        /* we probably need to scan through all the HT starting from the
        hashed slot location to be more successful... */
        //while (H[probe].key != NULL && strcmp(target.key, H[probe].key) && pc <= HASHSIZE/2)
        while (H[probe].used != 0 && strcmp(target.key, H[probe].key) && pc <= HASHSIZE/2)
        {
                pc++;
                probe = (probe + increment) % HASHSIZE;
                increment += 2;

        }

        //if (H[probe].key == NULL)
        if (H[probe].used == 0)
        {
        perror("Null entry! Not found!\n");
                return -1;
        }
        else if (strcmp(target.key, H[probe].key) == 0) /* if the two keys match */
                return probe;
        else {
        perror("Hash table is full or undefined error !\n");
                return -1;
             }
			return 0;
}

void printHashDistribution (HashTable H)
{
        int i, u;
        u = 0;
        for (i = 0; i < HASHSIZE; i++) {
                //if (H[i].key != NULL){
                if (H[i].used != 0){
                        u++;
                fprintf(logfile, "hashed to slot: %d\n", i);
                fprintf(logfile, "key is: %s\n", H[i].key);
                                     }  
                                       }
                fprintf(logfile, "Total number of used slots is: %d\n", u);
		fprintf(logfile, "HCD is: %d\n", HCD);
}

/* function to construct a key out of 4 tuples of the BGPMON_SEQ node:
 id, seq_num,  the timestamp and datetime from the TIME node */
int constructHash(xmlNode * a_node, HashEntry Item, HashTable ht)
{
	//xmlChar * id_buf = NULL;
	xmlChar id_buf[64];
	//xmlChar * seq_num_buf = NULL;
	xmlChar seq_num_buf[64];
	//xmlChar * timestamp_buf = NULL;
	xmlChar timestamp_buf[64];
	xmlChar datetime_buf[64];

        char key[512];
        char separator[] = ":";

	char random[128];

	static int t = 0;

	/* lets fill with blanks */
	strcpy((char *) id_buf, "");
	strcpy((char *) seq_num_buf, "");
	strcpy((char *) timestamp_buf, "");
	strcpy((char *) datetime_buf, "");
	strcpy(key, "");
	strcpy(random, "");

	//id_buf = get_id(a_node);
	get_id(a_node, id_buf);
	fprintf(logfile, "constructHash: id_buf is:%s\n", id_buf);

	//seq_num_buf = get_seq_num(a_node);
	get_seq_num(a_node, seq_num_buf);
	fprintf(logfile, "constructHash: seq_num_buf is:%s\n", seq_num_buf);

	//timestamp_buf =	(xmlChar *) get_timestamp(a_node);
	get_timestamp(a_node, timestamp_buf);
	fprintf(logfile, "constructHash: timestamp_buf is:%s\n", timestamp_buf);

	get_datetime(a_node, datetime_buf);
	fprintf(logfile, "constructHash: datetime_buf is:%s\n", datetime_buf);

	/* lets create the hash key */
	if (xmlStrlen(id_buf) > 0 && xmlStrlen(seq_num_buf) > 0 && xmlStrlen(timestamp_buf) > 0 && xmlStrlen(datetime_buf) > 0)
	{
	/*
	ran = new_rand();
        strcpy (random, dict[ran]);
	*/
		strcpy (key, (char *) id_buf);
		//strcat (key, random);
		strcat (key, separator);

	/*
	ran = new_rand();
        strcpy(random, "");
        strcpy (random, dict[ran]);
	*/
		strcat (key, (char *) seq_num_buf);
		//strcat (key, random);
		strcat (key, separator);

	/*
        ran = new_rand();
        strcpy(random, "");
        strcpy (random, dict[ran]);
	*/
		strcat (key, (char *) timestamp_buf);
		strcat (key, separator);
		strcat (key, (char *) datetime_buf);
		//strcat (key, random);
        	//strcpy(random, "");
		fprintf(logfile, "constructHash: concatenated key is: %s\n", key);
		t = 1;//indicate success
	}//end of all 4 elements collected
		if (t == 1)
		{
		strcpy (Item.key, key);
		if (insertHash (ht, Item) == -1)
		{
		strcpy (Item.key, "");
			t = 0;
			return -1;
		} else {
		strcpy (Item.key, "");
			t = 0;
			return 0;
		       }
		}//end of collection success
			return 2; //indicate that BGPMON_SEQ is not present in the message
}

int generateDict (void)
{
int ran = -1;
int i;
        for (i=0; i < 1024; i++)
        {
        strcpy(dict[i], "");
        ran = new_rand();
        sprintf(dict[i], strerror(ran));
        //sprintf(dict[i], "%d", ran);
        }
        return 0;
}

void random_init (void)
{
        srand((unsigned) time (NULL));
}

int new_rand (void)
{
        int max_number = 1022;
        return rand() % max_number + 1;
}


int get_id(xmlNode * a_node, xmlChar * buffer)
{
	xmlNode *cur_node = NULL;
	xmlChar * id_buf = NULL;
	int id;
	int i, j;
	static int f = 0;

		//lets do the search for the 1st attribute in the tree
	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
         fprintf(logfile, "get_id: 1st loop: Element name: %s\n", cur_node->name);
         fprintf(logfile, "get_id: 1st loop: value: %s\n", cur_node->content);

		if (f == 1)  {
	if (cur_node->properties != NULL) {
        if (cur_node->properties->type == XML_ATTRIBUTE_NODE) {
	
		//just testing....
                if (id_buf == NULL)
		id_buf = xmlGetProp(cur_node, (xmlChar *) "id");

                if (id_buf == NULL)
			continue;

                if (id_buf != NULL)
		{
		id = atoi((char *)id_buf);
         fprintf(logfile, "get_id: id is: %d\n", id);
         fprintf(logfile, "get_id: id is: %s\n", id_buf);
		xmlStrPrintf(buffer, xmlStrlen(id_buf)+1, id_buf);
		}
			f = 0;
                    xmlFree(id_buf);
			return 0;
										}//end of type == XML_ATTRIBUTE_NODE	
			    							     }//end of properties != NULL
										}//end of if (f == 1)
		if (f == 0)
		    f = 1;
								       
				 }//end of type != 0
	if (get_id(cur_node->children, buffer) == 0){
                        return 0;
                                                   }
											    }//end of for loop
                        return -1;
}

//xmlChar * get_seq_num(xmlNode * a_node)
int get_seq_num(xmlNode * a_node, xmlChar * buffer)
{
	xmlNode *cur_node = NULL;
	xmlChar * seq_num_buf = NULL;
	int seq_num;
	int i, j;
	static int f = 0;

		//lets do the search for the 2nd attribute in the tree
	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
         fprintf(logfile, "get_seq_num: 2nd loop: Element name: %s\n", cur_node->name);
         fprintf(logfile, "get_seq_num: 2nd loop: value: %s\n", cur_node->content);

		if (f == 1)  {
	if (cur_node->properties != NULL) {
        if (cur_node->properties->type == XML_ATTRIBUTE_NODE) {
	
		//just testing....
                if (seq_num_buf == NULL)
		seq_num_buf = xmlGetProp(cur_node, (xmlChar *) "seq_num");

                if (seq_num_buf == NULL)
			continue;

                if (seq_num_buf != NULL)
		{
		seq_num = atoi((char *)seq_num_buf);
         fprintf(logfile, "get_seq_num: seq_num is: %d\n", seq_num);
         fprintf(logfile, "get_seq_num: seq_num is: %s\n", seq_num_buf);
		xmlStrPrintf(buffer, xmlStrlen(seq_num_buf)+1, seq_num_buf);
		}
			f = 0;
                    xmlFree(seq_num_buf);
			return 0;
										}//end of type == XML_ATTRIBUTE_NODE	
			    							     }//end of properties != NULL
										}//end of if (f == 1)
		if (f == 0)
		    f = 1;
								       
				 }//end of type != 0
	if (get_seq_num(cur_node->children, buffer) == 0){
                        return 0;
                                                   }

											    }//end of for loop
                        return -1;
}

//xmlChar * get_timestamp(xmlNode * a_node)
int get_timestamp(xmlNode * a_node, xmlChar * buffer)
{
	xmlNode *cur_node = NULL;
	xmlChar * timestamp_buf = NULL;
	int timestamp;
	int i, j;
	static int f = 0;

		//lets do the search for the 3rd attribute in the tree
	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
         fprintf(logfile, "get_timestamp: 3rd loop: Element name: %s\n", cur_node->name);
         fprintf(logfile, "get_timestamp: 3rd loop: value: %s\n", cur_node->content);

		if (f == 1)  {
	if (cur_node->properties != NULL) {
        if (cur_node->properties->type == XML_ATTRIBUTE_NODE) {
	
		/*
                if (id_buf == NULL)
		id_buf = xmlGetProp(cur_node, (xmlChar *) "id");
                if (seq_num_buf == NULL)
		seq_num_buf = xmlGetProp(cur_node, (xmlChar *) "seq_num");
                if (timestamp_buf == NULL)
		timestamp_buf = xmlGetProp(cur_node, (xmlChar *) "timestamp");
		*/


		//just testing....
                if (timestamp_buf == NULL)
		timestamp_buf = xmlGetProp(cur_node, (xmlChar *) "timestamp");

                if (timestamp_buf == NULL)
			continue;

                if (timestamp_buf != NULL)
		{
		timestamp = atoi((char *)timestamp_buf);
         fprintf(logfile, "get_timestamp: timestamp is: %d\n", timestamp);
         fprintf(logfile, "get_timestamp: timestamp is: %s\n", timestamp_buf);
			xmlStrPrintf(buffer, xmlStrlen(timestamp_buf)+1, timestamp_buf);
		}
			f = 0;
                    xmlFree(timestamp_buf);
                        return 0;
										}//end of type == XML_ATTRIBUTE_NODE	
			    							     }//end of properties != NULL
										}//end of if (f == 1)
		if (f == 0)
		    f = 1;
								       
				 }//end of type != 0
	if (get_timestamp(cur_node->children, buffer) == 0){
                        return 0;
                                                   }

											    }//end of for loop
                        return -1;
}

int get_datetime(xmlNode * a_node, xmlChar * buffer)
{
	xmlNode *cur_node = NULL;
	xmlChar * datetime_buf = NULL;
	int datetime;
	int i, j;
	static int f = 0;

		//lets do the search for the 3rd attribute in the tree
	for (i=0, j = 0, cur_node = a_node; cur_node; i++, j++, cur_node = cur_node->next) {

        if (cur_node->type != 0) {
         fprintf(logfile, "get_datetime: 3rd loop: Element name: %s\n", cur_node->name);
         fprintf(logfile, "get_datetime: 3rd loop: value: %s\n", cur_node->content);

		if (f == 1)  {
	if (cur_node->properties != NULL) {
        if (cur_node->properties->type == XML_ATTRIBUTE_NODE) {
	
		/*
                if (id_buf == NULL)
		id_buf = xmlGetProp(cur_node, (xmlChar *) "id");
                if (seq_num_buf == NULL)
		seq_num_buf = xmlGetProp(cur_node, (xmlChar *) "seq_num");
                if (timestamp_buf == NULL)
		timestamp_buf = xmlGetProp(cur_node, (xmlChar *) "timestamp");
		*/


		//just testing....
                if (datetime_buf == NULL)
		datetime_buf = xmlGetProp(cur_node, (xmlChar *) "datetime");

                if (datetime_buf == NULL)
			continue;

                if (datetime_buf != NULL)
		{
		datetime = atoi((char *)datetime_buf);
         fprintf(logfile, "get_datetime: datetime is: %d\n", datetime);
         fprintf(logfile, "get_datetime: datetime is: %s\n", datetime_buf);
			xmlStrPrintf(buffer, xmlStrlen(datetime_buf)+1, datetime_buf);
		}
			f = 0;
                    xmlFree(datetime_buf);
                        return 0;
										}//end of type == XML_ATTRIBUTE_NODE	
			    							     }//end of properties != NULL
										}//end of if (f == 1)
		if (f == 0)
		    f = 1;
								       
				 }//end of type != 0
	if (get_datetime(cur_node->children, buffer) == 0){
                        return 0;
                                                   }

											    }//end of for loop
                        return -1;
}

#endif	/* HASHLIB_H */