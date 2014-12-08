/************************************************************************/
/*   PROGRAM NAME: server2.c  (works with client.c)                     */
/*                                                                      */
/*   Server creates a socket to listen for the connection from Client   */
/*   When the communication established, Server echoes data from Client */
/*   and writes them back.                                              */
/*                                                                      */
/*   Using socket() to create an endpoint for communication. It         */
/*   returns socket descriptor. Stream socket (SOCK_STREAM) is used here*/
/*   as opposed to a Datagram Socket (SOCK_DGRAM)                       */  
/*   Using bind() to bind/assign a name to an unnamed socket.           */
/*   Using listen() to listen for connections on a socket.              */
/*   Using accept() to accept a connection on a socket. It returns      */
/*   the descriptor for the accepted socket.                            */
/*                                                                      */
/*   To run this program, first compile the server_ex.c and run it      */
/*   on a server machine. Then run the client program on another        */
/*   machine.                                                           */
/*                                                                      */
/*   LINUX:      gcc -o server2  server2.c -lnsl                         */
/*                                                                      */
/************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>  /* define socket */
#include <netinet/in.h>  /* define internet socket */
#include <netdb.h>       /* define internet socket */
#include <pthread.h>
#include <string.h>
#include <signal.h>

#define SERVER_PORT 9999        /* define a server port number */

void *handleClient(void *params)
{
    printf("made it!\n");
}

int main()
{
    int sd, ns, k, pid;
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct sockaddr_in client_addr = { AF_INET };
    int client_len = sizeof( client_addr );
    char buf[512], *host;

    /* create a stream socket */
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
	   perror( "server: socket failed" );
	   exit( 1 );
    }
    
    /* bind the socket to an internet port */
    if( bind(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 )
    {
	   perror( "server: bind failed" );
	   exit( 1 );
    }
    
    while (1)
    {
        /* listen for clients */
        if( listen( sd, 1 ) == -1 )
        {
           perror( "server: listen failed" );
           exit( 1 );
        }

        printf("SERVER is listening for clients to establish a connection\n");
        if( ( ns = accept( sd, (struct sockaddr*)&client_addr,
                           &client_len ) ) == -1 )
        {
            perror( "server: accept failed" );
            exit( 1 );
        }

        printf("accept() successful.. a client has connected! waiting for a message\n");

        // doesn't work when this is uncommented
        // spin new thread
        // pthread_t newThread;
        // pthread_create(&newThread, NULL, handleClient, (void*)&sd);
    }

    return(0);
}
