/*
Copyright (c) 2012-2014  Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 * load testing program - opens regular file system  XML data file 
 * and streams XML messages to the client at a local IO speed.
*/

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

#define MAX_LINE 4096 
#define MAX_PENDING 2

        int sock, new_sock; 
	char buff[64*MAX_LINE];
	int fd;
	char filename[] = "bgpdata.txt";
	const char start_next[] = "<BGP_MESSAGE length=\"00002275\"";
	const char client_intro[] = "<xml><BGP_MESSAGE length=\"00000128\" version=\"0.2\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.2\" type_value=\"0\" type=\"MESSAGE\"></BGP_MESSAGE>";

int main (int argc, char *argv[])

{
        struct hostent *hp;
        struct sockaddr_in sin;
        struct sockaddr_in client_sin;
        socklen_t len;
	int p = 5000;

//build address data structure
//
sin.sin_family = AF_INET;
sin.sin_addr.s_addr = INADDR_ANY;
sin.sin_port = htons(p);
memset(&sin.sin_zero, '\0', sizeof(&sin.sin_zero));

        sock_tcp();

if ((bind(sock, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
	perror("can't bind to a socket!"); 
        exit(1);
}
	len = sizeof(client_sin);	

	if (listen(sock, MAX_PENDING) < 0 ) {
	perror("can't start listening on a socket!"); 
        exit(1);
	}

if ((fd = open (filename, O_RDONLY, S_IRUSR)) < 0)
{
perror("open");
exit(1);
}
	if ((new_sock = accept(sock, (struct sockaddr *)&sin, &len)) < 0 ) 
		{
	perror("can't accept on a socket!"); 
        exit(1);
		}
	//write introductory bgp message with <xml> tag to order the reading correctly in the client
write(new_sock, client_intro, strlen((char *)client_intro));

	while(1) {
		
		sendFile();

		 }
		close(fd);
		close(new_sock);
}//end of main	

int sock_tcp (void) 
{
int v = 1;
 if ((sock = socket (PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("can't open a socket!");
        exit(1);
        }
	//lets reuse the socket!
setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));
setsockopt(new_sock, SOL_SOCKET, SO_REUSEADDR, &v, sizeof(v));

}

int sendFile(void) 
{
char buf[64*MAX_LINE];
int r, w;
int i,j,z;
char m[8];
char mm[9];
int start_len_next;
int msg_len;
char xml_start [] = "\n<BGP_MESSAGE length=\"";
char xml_start0 [] = "<BGP_MESSAGE length=\"";

memset(m, '\0', sizeof(m));
memset(mm, '\0', sizeof(mm));
memset(buff, '\0', sizeof(buff));
memset(buf, '\0', sizeof(buf));

strcpy(buf, "");
strcpy(buff, "");
/*
if ((r = read(fd, buff, strlen(start_next))) < 0)
{
perror("read");
exit(1);
}
*/

r = read(fd, buff, strlen(start_next)); 
if (r < 0)
{
perror("read");
exit(1);
}


if (strncmp(xml_start, buff, strlen(xml_start)) == 0 || strncmp(xml_start0, buff, strlen(xml_start0)) == 0) ;
else {
    return -1;
     }

strcpy(buf, buff);
start_len_next = strlen(buff);

//extract the real length
for (i=21, j=0; i<start_len_next, j < 8; i++, j++) {
        m[j] = buf[i];
                                                   }
strncpy(mm, m, 7);
mm[7] = m[7];
mm[8] = 'x';//lets put a guard against accidental 0 in the string that might make a length value 10 times bigger...

msg_len = atoi(mm);

strcpy(buff, "");
r = read(fd, buff, msg_len - start_len_next);
strncat(buf, buff, msg_len - start_len_next); 

write (new_sock, buf, msg_len);

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
exit(1);
}
} 
