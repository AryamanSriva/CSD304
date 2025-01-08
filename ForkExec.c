#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>  

int main() {
    pid_t pid;

    printf("fork program starting\n");
    pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0) {
        printf("This is the child process\n");
        exit(0);
    }
    else {
        printf("This is the parent process, waiting for child\n");
        wait(NULL);  
        printf("Child process finished\n");
    }

    return 0;
}
