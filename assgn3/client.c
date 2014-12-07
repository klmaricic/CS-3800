/************************************************************************/ 
/*   PROGRAM NAME: client.c  (works with serverX.c)                     */ 
/*                                                                      */ 
/*   Client creates a socket to connect to Server.                      */ 
/*   When the communication established, Client writes data to server   */ 
/*   and echoes the response from Server.                               */ 
/*                                                                      */ 
/*   To run this program, first compile the server_ex.c and run it      */ 
/*   on a server machine. Then run the client program on another        */ 
/*   machine.                                                           */ 
/*                                                                      */ 
/*   COMPILE:    gcc -o client client.c -lnsl -lpthread                 */
/*                                                                      */ 
/************************************************************************/ 
 
#include <stdio.h> 
#include <sys/types.h> 
#include <sys/socket.h>  /* define socket */ 
#include <netinet/in.h>  /* define internet socket */ 
#include <netdb.h>       /* define internet socket */ 
#include <stdlib.h>
#include <string.h>
 
#define SERVER_PORT 9999     /* define a server port number */ 
 
// read messages from server and print on terminal
void *listenForMessages(void *void_ptr)
{
    char buf[512];

    int *sd = (int *)void_ptr;

    while (read(sd, buf, sizeof(buf)))
    {
        printf("%s\n", buf);
    }
}

int main( int argc, char* argv[] ) 
{
    int sd; 
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) }; 
    char buf[512]; 
    struct hostent *hp; 
    char hostname[256];
    char username[256];

    /* get the host */ 
    fgets(hostname, sizeof hostname, stdin);

    if( ( hp = gethostbyname( hostname ) ) == NULL ) 
    { 
	printf( "%s: %s unknown host\n", argv[0], hostname ); 
	exit( 1 ); 
    } 
    bcopy( hp->h_addr_list[0], (char*)&server_addr.sin_addr, hp->h_length ); 
 
    /* create a socket */ 
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 ) 
    { 
	perror( "client: socket failed" ); 
	exit( 1 ); 
    } 
 
    /* connect a socket */ 
    if( connect( sd, (struct sockaddr*)&server_addr, 
		 sizeof(server_addr) ) == -1 ) 
    { 
	perror( "client: connect FAILED:" ); 
	exit( 1 ); 
    }

    fgets(username, sizeof username, stdin);
 
    // send a message containing username to server
    write(sd, username, sizeof(username));

    // create new thread to listen for messages from the server
    pthread_t readThread;
    pthread_create(&readThread, NULL, listenForMessages, &sd)

    // we will use Main thread to accept user input and check for exit conditions and write to the server
    while( gets(&buf) != EOF) 
    {
        // [TODO] check for exit conditions
        // [TODO] close this thread when done
        // pthread_join(readThread, NULL);
        write(sd, buf, sizeof(buf)); 
    }
 
    close(sd); 
    return(0); 
} 