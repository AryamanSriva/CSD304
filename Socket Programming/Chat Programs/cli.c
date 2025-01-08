#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

int sockid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void display_prompt() {
    pthread_mutex_lock(&mutex);
    printf("Client: ");
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
}

void *receive_messages(void *arg) {
    char msg[1000];
    while (1) {
        int recvid = recv(sockid, msg, sizeof(msg), 0);
        if (recvid < 0) {
            perror("Error receiving message");
            break;
        }
        pthread_mutex_lock(&mutex);
        printf("\nServer: %s", msg);
        fflush(stdout);
        pthread_mutex_unlock(&mutex);

        if (strncmp("bye", msg, 3) == 0) {
            printf("Server disconnected. Ending session...\n");
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
        send(sockid, msg, strlen(msg), 0);

        if (strncmp("bye", msg, 3) == 0) {
            printf("Ending session...\n");
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in servaddr;
    sockid = socket(AF_INET, SOCK_STREAM, 0);

    if (sockid < 0) {
        perror("Error creating socket");
        return 1;
    }

    bzero((char*)&servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(6000);

    int connectid = connect(sockid, (struct sockaddr*)&servaddr, sizeof(struct sockaddr_in));
    if (connectid < 0) {
        perror("Error connecting to server");
        close(sockid);
        return 1;
    }

    pthread_t send_thread, receive_thread;

    pthread_create(&send_thread, NULL, send_messages, NULL);
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(sockid);
    return 0;
}