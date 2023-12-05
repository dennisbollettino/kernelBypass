#include thread_utils.h
#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

void start_client() {
    // Create a UDP socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    // Server address information
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = inet_addr("10.0.0.23");

    // Data to be sent (32-byte payload size)
    const char* message = "0123456789ABCDEFGHIJKLMNOPQRSTUV";  // Adjust the message as needed
    int message_length = strlen(message);

    // Prepare the message structure
    iovec iov[1];
    iov[0].iov_base = (void*)message;
    iov[0].iov_len = message_length;

    msghdr message_header;
    message_header.msg_name = (void*)&server_address;
    message_header.msg_namelen = sizeof(server_address);
    message_header.msg_iov = iov;
    message_header.msg_iovlen = 1;

    // Sending 32-byte UDP packets in a loop
    while (1) {
        // Send the message to the server using sendmsg
        sendmsg(client_socket, &message_header, 0);

        std::cout << "Packet sent\n";
    }

    // Close the socket when done
    close(client_socket);
}

int main() {
    createAndStartThread(1, "client", start_client)
    return 0;
}
