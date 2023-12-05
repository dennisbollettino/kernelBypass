#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000

using namespace std;

int start_server() {
    char buffer[32];
    int serv_socket, bytes;
    struct iovec iov[1];
    struct msghdr msg;
    struct sockaddr_in server_addr;
    

    if ((serv_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    memset(&msg, 0, sizeof(msg));
    memset(iov, 0, sizeof(iov));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(serv_socket, (const struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0) {
        perror("bind failed");
        return -1;
    }

    iov[0].iov_base = buffer;
    iov[0].iov_len = sizeof(buffer);

    msg.msg_name = (void*)&server_addr;
    msg.msg_namelen = sizeof(server_addr);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    while(true) {
        if ((bytes = recvmsg(serv_socket, &msg, 0)) < 0) {
            perror("recvmsg error");
            return -1;
        }
        
        cout << "Packet received" << endl;
        cout << "The message was: " << iov[0].iov_base << endl;
    }

    return 0;
}

int main() {
    if (start_server() < 0) {
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
