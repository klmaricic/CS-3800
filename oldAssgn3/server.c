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
/*   LINUX:      gcc -pthread -o server server.c                          */
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

int clientSDs[10];
int keepRunning = 1;

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void INThandler(int sig)
{
    printf("Server shutting down in 10 seconds!\n");
	keepRunning = 0;
}

void *handleClient(void *params)
{
    char buf[512];
    int *sd = (int *)params;
    char username[256];

    // read in username
    read(*sd, username, sizeof(username));

    // welcome message
    char message[256] = "Welcome";
    write(*sd, message, sizeof(message));

    // write to all clients that a new user has entered the room
    strcpy(message, username);
    strcat(message, " has entered the room\n");
    int i;
    for (i = 0; i < sizeof(clientSDs); i++)
    {
        int temp = clientSDs[i];
        if (temp != *sd)
        {
            write(temp, message, sizeof(message));
        }
    }

    while (read(sd, buf, sizeof(buf)))
    {
		printf("Reached while read \n");
        // check for special messages
        if (strcmp(buf, "/exit")==0 || strcmp(buf,"/quit")==0 || strcmp(buf, "/part")==0)
        {
            // print goodbye message
			printf("Reached manual quit test \n");
            strcpy(message, "Goodbye!");
            write(*sd, message, sizeof(message));
			
			//write to all clients that a user has left the room
            strcpy(message, username);
            strcat(message, " has left the room\n");
            for (i = 0; i < sizeof(clientSDs); i++)
            {
                int temp = clientSDs[i];
                if (temp != *sd)
                {
                    write(temp, message, sizeof(message));
                }
            }
            
            // put mutex on client array
            pthread_mutex_lock(&client_mutex);

            // [TODO] remove self from array


            // release mutex on client array
            pthread_mutex_unlock(&client_mutex);
            close(*sd);

            return;
        }

        strcpy(message, username);
        strcat(message, ": ");
        strcat(message, buf);

        // print message on server terminal
        printf("%s",message);

        // send message out to all other clients
        for (i = 0; i < sizeof(clientSDs); i++)
        {
            int temp = clientSDs[i];
            if (temp != *sd)
            {
                write(temp, buf, sizeof(buf));
            }
        }
    }
}

int main()
{
    int sd, ns, k, pid;
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct sockaddr_in client_addr = { AF_INET };
    int client_len = sizeof( client_addr );
    char buf[512], *host;
    pthread_t clients[10];
    // int clientSDs[10];
	
    signal(SIGINT, INThandler);
	
	while(keepRunning == 1)
	{
		printf("Reached 0\n");
    /* create a stream socket */
    if( ( sd = socket( AF_INET, SOCK_STREAM, 0 ) ) == -1 )
    {
	printf("Reached 1 \n");
       perror( "server: socket failed" );
       exit( 1 );
    }

    /* bind the socket to an internet port */
    if( bind(sd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1 )
    {
	printf("Reached 2 \n");
       perror( "server: bind failed" );
       exit( 1 );
    }

    /* listen for clients */
    if( listen( sd, 1 ) == -1 )
    {
	printf("Reached 3\n");
       perror( "server: listen failed" );
       exit( 1 );
    }
	printf("Reached before start message\n");
    printf("SERVER is listening for clients to establish a connection LOOK HERE blah\n");
	printf("Reached after start message\n");
    while ( ( ns = accept(sd, (struct sockaddr*)&client_addr, &client_len )) )
    {
        if ( ns == -1)
        {
		printf("Reached 4\n");
            perror( "server: accept failed" );
        }
printf("Reached in while loop\n");
        // connection is good
        // spin a new thread for this client
        pthread_t newThread;
        //pthread_create(&newThread, NULL, handleClient, &sd);
		printf("Reached in while loop 2222222\n");
    }
	printf("Reached after loop\n");


    // if ( (pid=fork()) == 0 )
    // {  /* child code begins */

    //     if( ( ns = accept( sd, (struct sockaddr*)&client_addr,
    //                    &client_len ) ) == -1 )
    //     {
    //         perror( "server: accept failed" );
    //         exit( 1 );
    //     }

    //     printf("Child accept() successful.. a client has connected to child ! waiting for a message\n");

    //     /* data transfer on connected socket ns */
    //     while( (k = read(ns, buf, sizeof(buf))) != 0)
    //     {    printf("SERVER(Child) RECEIVED: %s\n", buf);
    //          write(ns, buf, k);
    //     }
    //     close(ns);
    //     close(sd);
    // }
    // else 
    // {   /* Parent code */
    //     if( ( ns = accept( sd, (struct sockaddr*)&client_addr,
    //                    &client_len ) ) == -1 )
    //     {
    //         perror( "server: accept failed" );
    //         exit( 1 );
    //     }

    //     printf("Parent accept() successful.. a client has connected to Parent! waiting for a message\n");

    //     // spin a new thread for this client
    //     pthread_t newThread;
    //     pthread_create(&newThread, NULL, handleClient, &sd);
    // }
    }
	
    close(ns);
    unlink(server_addr.sin_addr);

    return(0);
}
