/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3500" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define DEBUG 

void printMenu();

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
	int sockfd, numbytes;  
	char buf[MAXDATASIZE],inputChar[MAXDATASIZE];
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
			perror("client: connect");
			close(sockfd);
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

	while(1) {
		printMenu();

		int i = 0,c =0;
		while ((c = getchar()) != '\n' && c != EOF) {
			inputChar[i] = c;
			i++;
    	}// Getinput from user
		inputChar[i] = '\0';

		#ifdef DEBUG
			printf("\nI typed %s \n", inputChar);
		#endif

		if (send(sockfd, inputChar, MAXDATASIZE-1, 0) == -1)
					perror("send");

		memset(inputChar,0,sizeof(inputChar)); // clear string buffer
		i = 0;
		
		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
			perror("recv");

		buf[numbytes] = '\0';

		#ifdef DEBUG
		printf("client: received '%s'\n",buf);
		#endif

		if(strcmp(buf,"q") == 0)	
			exit(1);
	}



	close(sockfd);
	

	return 0;
}

void printMenu() {
	printf("Main Menu\n");
	printf("------------------------------------------------------------------------------\n");
	printf("l: List: List the contents of the directory of the server\n");
	printf("c: Check <file name>: Check if the server has the file named <file name>. \n");
	printf("p: Display <file name>: Check if the server has the file named <file name>.\n");
	printf("d: Download <file name>: \n");
	printf("q: Quit\n");
	printf("-------------------------------------------------------------------------------\n");
	printf("Enter command :");
}