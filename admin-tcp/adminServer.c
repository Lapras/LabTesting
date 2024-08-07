#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#define MAX 80 
#define PORT 8080 
#define SA struct sockaddr 

// Function designed for chat between client and server. 
void adminServer(int connfd) 
{ 
    const char intro[] = "Welcome to the admin server! Please login";
    const char fail[] = "You have failed login. Try again.";
    const char success[] = "Congratulations on logging in. Type 'help' for a list of commands";
    const char help[] = "flag: Prints flag \n exit: exits connection \n";
	const char buff[MAX]; 
	int n; 

    write(connfd, intro, sizeof(intro));

    n = read(connfd, buff, MAX);
    if(n < 0) {
        printf("error reading from socket");
    }
    const char *pass = getenv("ADMINPASS");
    const char *flag = getenv("ADMINFLAG");
    if(strcmp(buff, pass) != 0) {
        write(connfd, fail, sizeof(fail));
        return;
    }
	// infinite loop for chat 
	for (;;) { 
		bzero(buff, MAX); 
        n = read(connfd, buff, MAX);
        if(n < 0) {
            printf("error reading from socket");
        }
        if(strcmp("flag", buff) == 0) {
            write(connfd, flag, 10);
        }

		// if msg contains "Exit" then server exit and chat ended. 
		if (strcmp("exit", buff) == 0) { 
			printf("Server Exit...\n"); 
			break; 
		} 
	} 
} 

void listen(struct sockaddr_in cli, int sockfd) {
    // Now server is ready to listen and verification 

	if ((listen(sockfd, 5)) != 0) { 
		printf("Listen failed...\n"); 
		exit(0); 
	} 
	else
		printf("Server listening..\n"); 
	int len = sizeof(cli); 

	// Accept the data packet from client and verification 
	int connfd = accept(sockfd, (SA*)&cli, &len); 
	if (connfd < 0) { 
		printf("server accept failed...\n"); 
		exit(0); 
	} 
	else
		printf("server accept the client...\n"); 

	// Function for chatting between client and server 
	adminServer(connfd); 
}

// Driver function 
int main() 
{ 
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1) { 
		printf("socket creation failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully created..\n"); 
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	servaddr.sin_port = htons(PORT); 

	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n"); 
    
    printf("Listening for clients...");

    for(;;) {
        listen(cli, sockfd);
    }    
	// After chatting close the socket 
	close(sockfd); 
}
