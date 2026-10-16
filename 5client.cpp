#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Compile and run in Terminal 2:
// g++ client.cpp -o client
// ./client

int main() {
    // 1. Create a UDP socket
    int client = socket(AF_INET, SOCK_DGRAM, 0);

    if (client < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Configure the server's address and port
    struct sockaddr_in saddr{};
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Send a packet to notify the server that the client is ready
    if (sendto(
            client, "HI", 2, 0,
            (struct sockaddr*)&saddr, sizeof(saddr)
        ) < 0) {
        perror("Initial message sending failed");
        close(client);
        return 1;
    }

    std::cout << "Waiting for server messages...\n";

    // 4. Receive and display messages from the server
    char buf[1024];

    while (true) {
        ssize_t bytes = recvfrom(
            client, buf, sizeof(buf) - 1, 0,
            nullptr, nullptr
        );

        if (bytes < 0) {
            perror("Receive failed");
            break;
        }

        // Add the null character to form a valid C-style string
        buf[bytes] = '\0';

        std::cout << "Server says: " << buf << std::endl;

        // End the chat when the server sends /quit
        if (std::string(buf) == "/quit") {
            break;
        }
    }

    // 5. Close the socket
    close(client);

    return 0;
}
