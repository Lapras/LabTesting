#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <unistd.h> // read(), write(), close()
#include <pthread.h>
#include <signal.h>

#define MAX 80
#define PORT 8082
#define MAXCONN 5
#define SA struct sockaddr 

int parentSig;

void handle_signal(int signal) {
	if (signal == SIGUSR1) {
		kill(parentSig, SIGUSR1);
	}
	if (signal == SIGTERM) {
		exit(0);
	}
 }

void printHex(char* buff, int buffsize) {
	int i;
	for (i = 0; i < buffsize; i++)
	{
		if (i > 0) printf(":");
		printf("%02X", buff[i]);
	}
	printf("\n");
}

// Function designed for chat between client and server. 
void *adminServer(int* fdPointer) 
{ 
    const char intro[] = "Welcome to the admin server! Please login \n";
    const char fail[] = "You have failed login. Bye. \n";
    const char success[] = "Congratulations on logging in. Type 'help' for a list of commands \n";
    const char help[] = "flag: Prints flag \n exit: exits connection \n";
	const char byby[] = "Have a nice day! \n";
	const char preFlag[] = "here's your flag! ";
	const char postFlag[] = " \n";
	const char unknown[] = "input unknown, try something else \n";
	const char watchdog[] = "good";
	char buff[80]; 
	int n; 
	int connfd = *fdPointer;

	char* pass = getenv("ADMINPASS");
    char* flag = getenv("ADMINFLAG");

	if(flag != NULL) {
		printf("pass is: %s \n", pass);
	} else {
		printf("ENV is not setup! \n");
	}

    write(connfd, intro, sizeof(intro));

	bzero(buff, MAX);

    n = read(connfd, buff, MAX);
	buff[strlen(buff)-1] = '\x00';
    if(n < 0) {
        printf("error reading from socket\n");
    }

	if(strcmp("wd", buff) == 0) {
		write(connfd, watchdog, sizeof(watchdog));
		close(connfd);
		return;
	} else if(strcmp(buff, pass) != 0) {
		printHex(buff, strlen(buff));
		printHex(pass, strlen(pass));
        write(connfd, fail, sizeof(fail));
		close(connfd);
        return;
	} else {
		printf("password accepted\n");
		write(connfd, success, sizeof(success));
	}
	// infinite loop for chat 
	for (;;) { 
		printf("reading input \n");
		bzero(buff, MAX); 
        n = read(connfd, buff, MAX);
		buff[strlen(buff)-1] = '\x00';
        if(n < 0) {
            printf("error reading from socket \n");
        }
        if(strcmp("flag", buff) == 0) {
			write(connfd, preFlag, sizeof(preFlag));
            write(connfd, flag, strlen(flag));
			write(connfd, postFlag, sizeof(postFlag));
			continue;
        }

		if(strcmp("help", buff) == 0) {
			write(connfd, help, sizeof(help));
			continue;
		}

		// if msg contains "Exit" then server exit and chat ended. 
		if (strcmp("exit", buff) == 0) { 
			printf("Server Exit...\n"); 
			write(connfd, byby, sizeof(byby));
			close(connfd);
			break; 
		} 

		write(connfd, unknown, sizeof(unknown));
	} 
} 

void listenFunc(struct sockaddr_in cli, int sockfd) {
	const char maxMsg[] = "We have reached the maximum number of connections, come back later.";
	const char error[] = "Internal server error. Contact sysadmin.";
	pthread_t connThreads[MAXCONN];
	int conns = 0;
    // Now server is ready to listen and verification 
	for(;;) {
		if(conns < MAXCONN-1) {
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
			if(conns >= MAXCONN-1) {
				write(connfd, maxMsg, sizeof(maxMsg));
				close(connfd);
			} else {
				printf("creating thread \n");
				pthread_t newThread;
				connThreads[conns] = newThread;
				if (pthread_create(&newThread, NULL, adminServer, (void*)&connfd) < 0) {
					printf("Could not create thread \n");
					write(connfd, error, sizeof(error));
				} else {
					conns ++;
				}
			}
			printf("thread created");
		} else {
			sleep(1);
		}
		
		for(int i = 0; i < conns; i++) {
			int res = pthread_tryjoin_np(connThreads[i], NULL);
			if (res == 0) {
				// Thread has finished and has been joined successfully
				printf("A client has closed connection.\n");
				conns --;
			}
		}// Function for chatting between client and server 
	}
}

void garbage() {
    char buf[3000];  
	return;  
}

// Driver function 
int main(int argc, char* argv[]) 
{ 
	signal(SIGUSR1, handle_signal);
	signal(SIGTERM, handle_signal);
	int sockfd, connfd, len; 
	struct sockaddr_in servaddr, cli; 

	parentSig = atoi(argv[1]);
	printf("parent signal is: %d \n", parentSig);

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
	} else {
		printf("Socket successfully binded..\n"); 
	}
    
    printf("Listening for clients... \n");

	listenFunc(cli, sockfd);
	// After chatting close the socket 
	close(sockfd); 
}