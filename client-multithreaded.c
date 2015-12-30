/*
 * client.c 
 * gcc client.c -lpthread -std=gnu99 (for linux systems)
 * ./a.out localhost 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT "9034" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define MAXNAMESIZE 25

//the thread function
void *receive_handler(void *);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    char message[MAXDATASIZE];
    char nickName[MAXNAMESIZE];
    //int *new_sockfd; //added
    int sockfd;//, numbytes;  
    //char buf[MAXDATASIZE];
    char sBuf[MAXDATASIZE]; //added
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
    
    //****EDIT STARTS HERE    
    puts("Nickname:");
    memset(&nickName, sizeof(nickName), 0);
    memset(&message, sizeof(message), 0); //clean message buffer
    fgets(nickName, MAXNAMESIZE, stdin);  //catch nickname
    //puts(message);
    
    //create new thread to keep receiving messages:
    pthread_t recv_thread;
    //new_sockfd = malloc(sizeof(int));
    //new_sockfd = sockfd;
    
    if( pthread_create(&recv_thread, NULL, receive_handler, (void*)(intptr_t) sockfd) < 0)
    {   //we passed (intptr_t) instead of (void*) sockfd to supress warnings
		//use structs if passing more than one argument
        perror("could not create thread");
        return 1;
    }    
    puts("Synchronous receive handler assigned");
    
    //send message to server:
    puts("Connected\n");
    puts("[Type '/quit' to quit]");
    while(strcmp(sBuf,"/quit") != 0)
    {
        memset(&sBuf, sizeof(sBuf), 0); //clean sendBuffer
        fgets(sBuf, 100, stdin); //gets(message);
        int count = 0;
        while(count < strlen(nickName))
        {
            message[count] = nickName[count];
            count++;
        }
        count--;
        message[count] = ':';
        count++;
        for(int i = 0; i < strlen(sBuf); i++)
        {
            message[count] = sBuf[i];
            count++;
        }
        message[count] = '\0';
        //puts(message);
        //Send some data
        //if(send(sockfd, sBuf , strlen(sBuf) , 0) < 0)
        if(send(sockfd, message, strlen(message), 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        memset(&sBuf, sizeof(sBuf), 0);
        
        /* receive message from client: 
         * we move this to a thread in order to get synchronous recv()s.
         */
    }
    
    //puts("Closing socket connection");
    pthread_join(recv_thread , NULL);
    close(sockfd);

    return 0;
}

//thread function
void *receive_handler(void *sock_fd)
{
    //int* sFd = (int*) sock_fd;
	int sFd = (intptr_t) sock_fd;
    char buffer[MAXDATASIZE];
    int nBytes;
    
    for(;;)
    {
        if ((nBytes = recv(sFd, buffer, MAXDATASIZE-1, 0)) == -1)
        {
            perror("recv");
            exit(1);
        }
        else
            buffer[nBytes] = '\0';
        printf("%s", buffer);
    }
}
