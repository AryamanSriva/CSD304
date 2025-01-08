#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int newsockid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void display_prompt() {
    pthread_mutex_lock(&mutex);
    printf("Server: ");
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
}

void *receive_messages(void *arg) {
    char msg[1000];
    while (1) {
        int recvid = recv(newsockid, msg, sizeof(msg), 0);
        if (recvid < 0) {
            perror("Error receiving message");
            break;
        }
        pthread_mutex_lock(&mutex);
        printf("\nClient: %s", msg);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);

        if (strncmp("bye", msg, 3) == 0) {
            printf("Client disconnected. Ending session...\n");
            break;
        }
        bzero(msg, 1000);
        display_prompt();
    }
    return NULL;
}

void *send_messages(void *arg) {
    char msg[1000];
    while (1) {
        display_prompt();
        fgets(msg, sizeof(msg), stdin);
        send(newsockid, msg, strlen(msg), 0);

        if (strncmp("bye", msg, 3) == 0) {
            printf("Ending session...\n");
            break;
        }
    }
    return NULL;
}

int main() {
    int sockid, bindid;
    struct sockaddr_in myaddr, client;
    int clientlen = sizeof(struct sockaddr_in);

    sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid < 0) {
        perror("Error creating socket");
        exit(1);
    }

    bzero((char*)&myaddr, sizeof(struct sockaddr_in));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(6000);

    bindid = bind(sockid, (struct sockaddr*)&myaddr, sizeof(struct sockaddr_in));
    if (bindid < 0) {
        perror("Error binding socket");
        close(sockid);
        exit(1);
    }

    listen(sockid, 5);
    newsockid = accept(sockid, (struct sockaddr*)&client, &clientlen);
    if (newsockid < 0) {
        perror("Error accepting connection");
        close(sockid);
        exit(1);
    }

    pthread_t send_thread, receive_thread;

    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(newsockid);
    close(sockid);
    return 0;
}