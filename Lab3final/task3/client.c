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
#define MAXDATASIZE 1000 // max number of bytes we can get at once
#define DEBUG 

void printMenu();
void printHelpMenu();

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

		char filename[MAXDATASIZE];

		if(inputChar[0] == 'l' || inputChar[0] == 'L'){
			if (send(sockfd, "l", MAXDATASIZE-1, 0) == -1)
				perror("send");
				
			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
				perror("recv");
			buf[numbytes] = '\0';
			printf("\nFiles in system are\n %s", buf);
		}// ls command

		else if(inputChar[0] == 'c' || inputChar[0] == 'C'){
			if (send(sockfd, "c", MAXDATASIZE-1, 0) == -1)
				perror("send");

			printf("What file would you like to check for?:\n");
			scanf("%s", &filename);
			while (getchar() != '\n') continue;

			if (send(sockfd, filename, MAXDATASIZE-1, 0) == -1)
				perror("send");

			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
				perror("recv");
	
            buf[numbytes] = '\0';

			if(!strcmp(buf, "yes"))
				printf("\nFile was found \n");

			else
				printf("\nFile was not found \n");

		}//Check file command
		
		else if(inputChar[0] == 'p' || inputChar[0] == 'P'){

			if (send(sockfd, "p", MAXDATASIZE-1, 0) == -1)
				perror("send");

			printf("What file would you like to print out?:\n");
			scanf("%s", &filename);
			while (getchar() != '\n') continue;

			if (send(sockfd, filename, MAXDATASIZE-1, 0) == -1)
				perror("send");

			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
				perror("recv");
			buf[numbytes] = '\0';

			if(!strcmp(buf,"no")){
				printf("\n ERROR:File not found\n");
			}
			else{
				printf("\nThe file contain the following text:\n %s", buf);
			}

		}

		else if(inputChar[0] == 'd' || inputChar[0] == 'D'){
			if (send(sockfd, "p", MAXDATASIZE-1, 0) == -1)
				perror("send");

			printf("What file would you like to Download?:\n");
			scanf("%s", &filename);
			while (getchar() != '\n') continue;

			memset(buf,0,sizeof(buf)); // clear string buffer

			if (send(sockfd, filename, MAXDATASIZE-1, 0) == -1)
				perror("send");

			if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) 
				perror("recv");
			buf[numbytes] = '\0';

			if(!strcmp(buf,"no")){
				printf("\n ERROR:File not found\n");
			}
			else{
				printf("File Downloaded from Server\n");
				printf("Save as: "); //client side filename
				scanf("%s", &inputChar);
				while (getchar() != '\n') continue;

				//download file from server
				FILE *fp;
				fp = fopen(inputChar, "r");
				if(fp != NULL){
					char choice[5];
					printf("File %s exists. Do you want to override? (y/n): ", filename);
					scanf("%4s", &choice);
				
					if (choice[0] != 'n')	{
					//Write file
						fp = fopen(inputChar, "w");
						if(NULL == fp){
							printf("Error Opening File");
							return 1;
						}
						else {
							fprintf(fp, buf);
							fclose(fp);
						}
					}
				}
				else { //File doesn't exist, create file with same name
					fp = fopen(inputChar, "w");
					if (fp == NULL) {
						printf("Error Opening File");
						return 1;
					}
					else {
						//fwrite("bufer", 1, 6, fp);
						fprintf(fp, buf);
						fclose(fp);
					}
				}
				
			}
		
		}

		else if(inputChar[0] == 'q' || inputChar[0] == 'Q'){
			if (send(sockfd, "q", MAXDATASIZE-1, 0) == -1)
				perror("send");
			exit(1);
		}
		else {
			printHelpMenu();
		}

		memset(inputChar,0,sizeof(inputChar)); // clear string buffer
		i = 0;
		
	}

	close(sockfd);
	return 0;
}

void printMenu() {
	printf("\n\nMain Menu\n");
	printf("ALL COMMAND ARE SINGLE TEXT CHARACTER\n");
	printf("------------------------------------------------------------------------------\n");
	printf("l: List \n");
	printf("c: Check <file name> \n");
	printf("p: Display <file name>\n");
	printf("d: Download <file name>\n");
	printf("q: Quit\n");
	printf("h: Help\n");
	printf("-------------------------------------------------------------------------------\n");
	printf("Enter command :");
}

void printHelpMenu() {
	printf("\n\nh: Help: List of all command\n");
	printf("	l: List: List the contents of the directory of the server\n");
	printf("	c: Check <file name>: Check if the server has the file named <file name>. \n");
	printf("	p: Display <file name>: Check if the server has the file named <file name>.\n");
	printf("	d: Download <file name>: Downloads <file name>.\n");
	printf("	q: Quit: End program and connection with host\n");
	printf("	h: Help: Display all available commands\n");
}