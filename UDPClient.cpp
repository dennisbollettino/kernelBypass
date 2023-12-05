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
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8000);
    server_address.sin_addr.s_addr = inet_addr("10.0.0.23");

    // Data to be sent (32-byte payload size)
    const char* message = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";  // Adjust the message as needed
    int message_length = strlen(message);

    // Sending 32-byte UDP packets in a loop
    for (int i = 0; i < 10; ++i) {
        // Send the message to the server
        sendto(client_socket, message, message_length, 0,
               (const struct sockaddr*)&server_address, sizeof(server_address));

        std::cout << "Packet " << i + 1 << " sent\n";
    }

    // Close the socket when done
    close(client_socket);
}

int main() {
    start_client();
    return 0;
}