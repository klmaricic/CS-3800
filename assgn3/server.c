/************************************************************************/
/*   PROGRAM NAME: server.c  (works with client.c)                     */
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
#include <unistd.h>

#define SERVER_PORT 9999        /* define a server port number */

typedef struct {
    struct sockaddr_in addr;
    int connfd;
} client_t;

client_t *clients[10];
pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handleClient(void *params)
{
    char buf[512];
    client_t *client = (client_t *)params;
    char username[256];

    read(client->connfd, username, sizeof(username));

    // welcome message
    char message[256] = "Welcome!\n";
    write(client->connfd, message, sizeof(message));

    // write to all clients that a new user has entered the room
    strcpy(message, username);
    strcat(message, " has entered the room\n");

    printf("%s", message);

    int e;
    for (e = 0; e < 10; e++)
    {
        if (clients[e] && clients[e]->connfd != client->connfd)
        {
            write(clients[e]->connfd, message, sizeof(message));
        }
    }

    while (read(client->connfd, buf, sizeof(buf)))
    {
        // check for special messages
        if (strcmp(buf, "/exit\n")==0 || strcmp(buf,"/quit\n")==0 || strcmp(buf, "/part\n")==0)
        {
            // print goodbye message
            strcpy(message, "Goodbye!\n");
            write(client->connfd, message, sizeof(message));
            
             // write to all clients that a new user has left the room
             strcpy(message, username);
             strcat(message, " has left the room\n");
             printf("%s", message);
             for (e = 0; e < 10; e++)
             {
                 if (clients[e] && clients[e]->connfd != client->connfd)
                 {
                     write(clients[e]->connfd, message, sizeof(message));
                 }
             }

            
            // put mutex on client array
            pthread_mutex_lock(&client_mutex);

            // remove self from array
            int i;
            for (i = 0; i < 10; i++)
            {
                if (clients[i])
                {
                    if (clients[i]->connfd == client->connfd)
                    {
                        clients[i] = NULL;
                        break;
                    }
                }
            }

            // release mutex on client array
            pthread_mutex_unlock(&client_mutex);

            close(client->connfd);

            return;
        }

        strcpy(message, username);
        strcat(message, ": ");
        strcat(message, buf);

        // print message on server terminal
        printf("%s",message);

        int u;
        for (u = 0; u < 10; u++)
        {
            if (clients[u] && clients[u]->connfd != client->connfd)
            {
                write(clients[u]->connfd, message, sizeof(message));
            }
        }
    }
}

void INThandler(int sig)
{
    char message[256];
    strcpy(message, "SERVER IS GOING DOWN IN 10 SECONDS\n");
    printf("%s", message);

    int i;
    for (i = 0; i < 10; i++)
    {
        if (clients[i])
        {
            write(clients[i]->connfd, message, sizeof(message));
        }
    }

    // wait 10 seconds
    sleep(10);

    // exit gracefully
    for (i = 0; i < 10; i++)
    {
        if (clients[i])
        {
            close(clients[i]->connfd);
        }
    }

    exit ( 1 );
}

int main()
{
    int sd, ns, k, pid;
    struct sockaddr_in server_addr = { AF_INET, htons( SERVER_PORT ) };
    struct sockaddr_in client_addr = { AF_INET };
    int client_len = sizeof( client_addr );
    char buf[512], *host;

    signal(SIGINT, INThandler);

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
    
    printf("Server running...\n");
    while (1)
    {
        /* listen for clients */
        if( listen( sd, 1 ) == -1 )
        {
           perror( "server: listen failed" );
           exit( 1 );
        }

        if( ( ns = accept( sd, (struct sockaddr*)&client_addr,
                           &client_len ) ) == -1 )
        {
            perror( "server: accept failed" );
            exit( 1 );
        }

        // spin new thread
        client_t *client = (client_t *)malloc(sizeof(client_t));
        client->addr = client_addr;
        client->connfd = ns;

        pthread_t newThread;
        pthread_create(&newThread, NULL, handleClient, (void*)client);
        
        int i;
        for (i = 0; i < 10; i++)
        {
            if (!clients[i])
            {
                clients[i] = client;
                break;
            }
        }
    }

    return(0);
}

