/*
 *  * Justin Gallagher
Project 3 - Multi-threaded Echo Server
Due Apr 6 by 1pm Points 10 Submitting a file upload Available after Mar 23 at 6am
CIS 3207 Assignment 3
Multi-Threaded Echo Server
10 points

[X] Launch Server (0.5 pts)
[X] Connect 3 Clients to the Server (0.5 pts)
[X] Send a single word (0.5 pts)
[X] Send a sequence of phrases (0.5 pts)
[X] Send terminator for shutdown of server thread (0.5 pts)
[X] Makefile (0.25 pts)
[X] Accept Multiple Simultaneous Connections (1 pt)
[X] Server Produces into connection Buffer (0.25 pts)
[ ] Worker Threads - Consume from connection Buffer and .75
[X] Worker threads - multiple worker threads simultaneously 1
[X] Only 1 Thread Accessing Queues at a Time (1 pt)
[X] Communication Between Worker Thread and its Client (1 pt)
[ ] Correct use of condition variables (0.75 pts)
[X] Program Design Description (0.75 pts)
[X] Testing, Results, and Debugging Description (0.75 pts)
[X] Results of Testing with many clients (0.5)
[X] Discussion and Analysis (0.75 pts)
9.00 / 11.25 Subtotal
Days Late
Final Grade (out of 10 points)

*/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

char *logData;
time_t t;
int connectedClients = 0;
char *DEFAULT_PORT, *DEFAULT_ADDRESS, *DEFAULT_TERMINATING_STRING;

/*
Declaration of thread condition variable
*/
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

/*
declaring mutex
*/
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;


struct serverParm {
    int connectionDesc;
};

void *serverThread(void *parmPtr);
void logDataToFile(char message[2000]);

/*
Main function - This will begin by setting the default values used for the application through getops. More detauls are
 below before each functions.
*/
int main(int argc, char *argv[]) {
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;
    int opt;
    DEFAULT_PORT = "6666";
    DEFAULT_ADDRESS = "127.0.0.1";
    DEFAULT_TERMINATING_STRING = "exit";


    if (argc < 2) {
        printf("Default port has been selected, 6666\n");
        printf("Default address has been selected, 127.0.0.1\n");

    }

    while ((opt = getopt(argc, argv, ":p:a:t:")) != -1) {

        switch (opt) {

            case 'p':
                /*This will bring in the port from the user if the -p flag is used*/
                DEFAULT_PORT = strdup(optarg);
                break;
            case ':':
                printf("-p option needs argument following (PORT/sin_port)\n");
                printf("-a option needs argument following (ADDRESS/sin_addr)\n");
                printf("-t option needs argument following (DEFAULT_TERMINATING_STRING)\n");

                exit(EXIT_FAILURE);
            case 'a':
                /*This will bring in the address from the user if the -a flag is used*/
                DEFAULT_ADDRESS = strdup(optarg);
                break;
            case 't':
                /*This will bring in the address from the user if the -a flag is used*/
                DEFAULT_TERMINATING_STRING = strdup(optarg);
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
            default:
                printf("error: Ya broke it\n"
                       "\n"
                       "Usage: ./server -p 6666 -a 127.0.0.1 -t exit\n");
                exit(EXIT_FAILURE);

        }
    }

/*
    optind is for the extra arguments which are not parsed
*/
    for (; optind < argc; optind++) {
        printf("extra arguments unknown: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }


    /*Create socket from which to read*/
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
        exit(1);
    } else {
        puts("Socket() was successful!");
    }



    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = inet_addr(DEFAULT_ADDRESS);
    myAddr.sin_port = htons(atoi(DEFAULT_PORT));

    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
        exit(1);
    } else {
        puts("bind() was successful!");
    }
    /* Start accepting connections */
    /* Up to 3 requests for connections can be queued... */

    if (listen(listenDesc, 3) != 0) {
        printf("listen() has failed...\n");
        exit(0);
    } else {
        printf("Listen() successful! Server listening...\n");
    }

    while (1) /* Do forever */ {
        while (1) {
            /* Wait for a client connection */
            connectionDesc = accept(listenDesc, NULL, NULL);
            /* Create a thread to actually handle this client */
            parmPtr = (struct serverParm *) malloc(sizeof(struct serverParm));
            parmPtr->connectionDesc = connectionDesc;
            if (pthread_create(&threadID, NULL, serverThread, (void *) parmPtr) != 0) {
                perror("Thread create error");
                close(connectionDesc);
                close(listenDesc);
                exit(1);
            }
            printf("Parent ready for another connection\n");
        }

    }
}


void *serverThread(void *parmPtr) {
#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[1025];
    /* Server thread code to deal with message processing */
    connectedClients++;
    printf("Connection made! Total clients connected:%d\n", connectedClients);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return (0);    /* Exit thread */
    }

    /* Receive messages from sender*/
    while ((recievedMsgLen = read(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf) - 1)) > 0) {
        recievedMsgLen[messageBuf] = '\0';
        printf("Message from client: %s\n", messageBuf);
        logDataToFile(messageBuf);

        if (!bcmp(messageBuf, DEFAULT_TERMINATING_STRING, strlen(DEFAULT_TERMINATING_STRING))) {
            if (write(PARMPTR->connectionDesc, "ECHO SERVICE COMPLETE\0", 22) < 0) {
                perror("Server: Kill failed");
                return (0);
            }
            connectedClients--;
            printf("Connection lost! Total clients connected:%d\n", connectedClients);
        }

        if (write(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf) - 1) < 0) {
            perror("Server: write error");
            return (0);
        }

    }


    close(PARMPTR->connectionDesc);
    free(PARMPTR);
    return (0);
}

void logDataToFile(char message[2000]) {
    FILE *f;
    //Received client_message as arg
    time(&t); // Set time

    f = fopen("server.txt", "a+");
    fprintf(f, "%s: received from client @ %s", message, ctime(&t));
    fclose(f);
}
