#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    char buff[32];
    const int port = 4321;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    if (fd < 0) {
        std::cerr << "Creating receiver socket failed" << std::endl;
        return -1;
    }

    sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind Failed" << std::endl;
        return -1;
    }

    socklen_t len = sizeof(clientAddr);
    int n = recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr*)&clientAddr, &len);
    buff[n] = '\0';
    std::cout << "Received: " << buff << std::endl;

    close(fd);

    return 0;
}
