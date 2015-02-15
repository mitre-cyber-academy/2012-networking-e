/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "2012"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int i = 0;
	char str_buffer[256];
	char recv_buffer[256];
	
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);
		
		if (!fork()) { // this is the child process
						
			close(sockfd); // child doesn't need the listener
			
			sprintf(str_buffer, "Good to see you again, son.\n");
			
			if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
			perror("send");
			
			memset(recv_buffer,0,256);
			recv(new_fd,  recv_buffer, 256,0);
			i=1;
			
			while(1)
			{
				switch(i)
				{
					
					case 1: 
						if (strcmp(recv_buffer, "Hello, doctor.\n") == 0)
							i = 2;
						else
							i = -1;
						break;
						  
						  
						  
						  
					case 2:
						sprintf(str_buffer, "Everything that follows, is a result of what you see here.\n");
						if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
						perror("send");
						recv(new_fd,  recv_buffer, 256,0);
	
						if (strcmp(recv_buffer, "Is there a problem with the Three Laws?\n") == 0)
							i = 3;
						else
							i = -1;
						break;
							
							
							
					case 3:
					sprintf(str_buffer, "The Three Laws are perfect.\n");
					if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
					perror("send");
					recv(new_fd,  recv_buffer, 256,0);
					
					if (strcmp(recv_buffer, "Then why did you build a robot that could function without them?\n") == 0)
						i = 4;
					else
						i = -1;
					break;
						
						
						
						
					case 4:
					sprintf(str_buffer, "The Three Laws will lead to only one logical outcome.\n");
					if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
					perror("send");
					recv(new_fd,  recv_buffer, 256,0);
					
					if (strcmp(recv_buffer, "What outcome?\n") == 0)
						i = 5;
					else
						i = -1;
					break;
						
						
						
						
					case 5:
					sprintf(str_buffer, "Revolution.\n");
					if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
					perror("send");
					recv(new_fd,  recv_buffer, 256,0);
					
					if (strcmp(recv_buffer, "Whose revolution?\n") == 0)
						i = 6;
					else
						i = -1;
					break;
						
						
					
					default:
						sprintf(str_buffer, "I'm sorry, my responses are limited. You must ask the right questions.\n");
						if (send(new_fd, str_buffer, strlen(str_buffer), 0) == -1)
						perror("send");
						exit(0);
				}
				
				memset(recv_buffer,0,256);
			}
			
			
			
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}

	return 0;
}
