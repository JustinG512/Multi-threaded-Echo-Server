/*
 * Justin Gallagher
Project 3 - Multi-threaded Echo Server
Due Apr 6 by 1pm Points 10 Submitting a file upload Available after Mar 23 at 6am
CIS 3207 Assignment 3
Multi-Threaded Echo Server
10 points

*/

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <getopt.h>


#define DEFAULT_LENGTH 4096 /*max text line length*/


/*
This is a simple client that will send lines to the requested server.
*/
int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[DEFAULT_LENGTH], recvline[DEFAULT_LENGTH];
    int opt;
    char *DEFAULT_PORT;
    char *DEFAULT_ADDRESS;
    DEFAULT_PORT = "6666";
    DEFAULT_ADDRESS = "127.0.0.1";


    if (argc < 2) {
        printf("Default port has been selected, 6666\n");
        printf("Default address has been selected, 127.0.0.1\n");

    }

    /*
    getops will return any data that user sends through flags.  This will overwrite the defaults user for the application
    */
    while ((opt = getopt(argc, argv, ":p:a:")) != -1) {

        switch (opt) {

            case 'p':
                /*This will bring in the port from the user if the -p flag is used*/
                DEFAULT_PORT = strdup(optarg);
                break;
            case ':':
                printf("-p option needs argument following (PORT/sin_port)\n");
                printf("-a option needs argument following (ADDRESS/sin_addr)\n");
                exit(EXIT_FAILURE);
            case 'a':
                /*This will bring in the address from the user if the -a flag is used*/
                DEFAULT_ADDRESS = strdup(optarg);
                break;
            case '?':
                printf("unknown option: %c\n", optopt);
                break;
            default:
                printf("error: Ya broke it\n"
                       "\n"
                       "Usage: ./client -p 6666 -a 127.0.0.1\n");
                exit(EXIT_FAILURE);

        }
    }

    // optind is for the extra arguments which are not parsed
    for (; optind < argc; optind++) {
        printf("extra arguments unknown: %s\n", argv[optind]);
        exit(EXIT_FAILURE);
    }



/*
    Create a socket for the client
*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        exit(2);
    } else {
        puts("Socket() was successful!");
    }

/*
Creation of the socket
*/
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(DEFAULT_ADDRESS);
    servaddr.sin_port = htons(atoi(DEFAULT_PORT));

/*
Connection of the client to the socket
*/
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("Problem in connecting to the server");
        exit(3);
    } else {
        printf("Connected!\n\n");
    }

    while (fgets(sendline, DEFAULT_LENGTH, stdin) != NULL) {

        send(sockfd, sendline, strlen(sendline), 0);

        if (recv(sockfd, recvline, DEFAULT_LENGTH, 0) == 0) {
            //error: server terminated prematurely
            perror("The server terminated prematurely");
            exit(4);
        }

        printf("%s", "Message ping from server: ");
        fputs(recvline, stdout);
        printf("\n");

        /*This will quit after sending message if quit is used*/
        if (!bcmp(recvline, "ECHO SERVICE COMPLETE", 21))
            break;

        printf("\nEnter a message:");


    }
    exit(0);
}