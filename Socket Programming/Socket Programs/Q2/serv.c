#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main() {
    int sockid, newsockid, bindid, recvid;
    struct sockaddr_in myaddr, client;
    char msg[1000];
    int clientlen = sizeof(client);
    int port_id = 6000;

    sockid = socket(AF_INET, SOCK_STREAM, 0);  // TCP connection
    if (sockid < 0) {
        printf("Error creating socket\n");
        return -1;
    }

    bzero((char*)&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port_id);

    bindid = bind(sockid, (struct sockaddr*)&myaddr, sizeof(myaddr));
    if (bindid < 0) {
        printf("Error binding socket\n");
        return -1;
    }

    listen(sockid, 5);  // Max 5 connections
    printf("Server is listening on port %d\n", port_id);

    newsockid = accept(sockid, (struct sockaddr*)&client, &clientlen);
    if (newsockid < 0) {
        printf("Error accepting connection\n");
        return -1;
    }

    for (int i = 0; i < 5; i++) {
        recvid = recv(newsockid, msg, sizeof(msg), 0);
        if (recvid < 0) {
            printf("Error receiving message\n");
        } else {
            printf("Received from client: %s\n", msg);
            send(newsockid, "ACK", sizeof("ACK"), 0);  // Send ACK
        }
    }

    close(newsockid);
    close(sockid);
    return 0;
}
