#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> // read(), write(), close()
#include <signal.h>
#include <sys/types.h> 

int sigStatus;

void handleSignal(int signal) {
    if(signal == SIGUSR1) {
        sigStatus = 1;
    }
}

int forkServer(int ownPID) {
    pid_t pid = fork();
    if(pid < 0) {
        printf("Failed to fork a server\n");
        // exit(0);
    } else if(pid == 0) {
        printf("execcing\n");
        char pidstring[12];
        snprintf(pidstring, sizeof(pidstring), "%d", ownPID);
        execlp("./adminServer", "adminserver", pidstring,(char*) NULL);
        exit(0);
    } else {
        return pid;
    }
}

void main() {
    printf("staring up TCP server... \n");
    fflush(NULL);
    //estbalish signal handler
    signal(SIGUSR1, handleSignal);
    //Fork and start adminServer as child
    pid_t servPID;

    pid_t ownPID = getpid();
    

    servPID = forkServer(ownPID);
    sleep(1);

    // Every 30 seconds, send SIGUSR1 to adminServer
    for (;;) {
        sigStatus = 0;
        kill(servPID, SIGUSR1);
        printf("signal sent\n");
        fflush(NULL);

        //if adminServer does not reply with SIGUSR1 in 5 seconds, kill admin server and restart the process
        sleep(5);
        if(sigStatus == 0) {
            printf("killnig and forking a new server \n");
            // kill adminServer
            kill(servPID, SIGTERM);

            // fork new adminServer
            servPID = forkServer(ownPID);
            fflush(NULL);
        } else {
            printf("SIG check OK\n");
            sigStatus = 0;
            fflush(NULL);
        }
        sleep(30);
    }

}