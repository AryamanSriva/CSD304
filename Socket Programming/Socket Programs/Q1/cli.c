#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  
#include <string.h>
#include <unistd.h>

int main() {
    int sockid, connectid, recvid;
    struct sockaddr_in servaddr;
    char msg[1000] = "hello";
    int port_id = 6000;

    sockid = socket(AF_INET, SOCK_STREAM, 0);
    if (sockid < 0) {
        printf("Error creating socket\n");
        return -1;
    }

    bzero((char*)&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");  
    servaddr.sin_port = htons(port_id);

    connectid = connect(sockid, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if (connectid < 0) {
        printf("Error connecting to server\n");
        return -1;
    }

    send(sockid, msg, sizeof(msg), 0);
    recvid = recv(sockid, msg, sizeof(msg), 0);
    if (recvid > 0) {
        printf("Received from server: %s\n", msg);
    }

    close(sockid);
    return 0;
}
