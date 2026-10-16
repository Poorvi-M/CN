#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Compile and run in Terminal 1:
// g++ server.cpp -o server
// ./server

int main() {
    // 1. Create a UDP socket
    int server = socket(AF_INET, SOCK_DGRAM, 0);

    if (server < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Configure the server address and port
    struct sockaddr_in saddr{};
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket to port 8080
    if (bind(server, (struct sockaddr*)&saddr, sizeof(saddr)) < 0) {
        perror("Bind failed");
        close(server);
        return 1;
    }

    std::cout << "Waiting for client..." << std::endl;

    // 3. Receive the client's first packet to obtain its address
    struct sockaddr_in caddr{};
    socklen_t len = sizeof(caddr);
    char buf[1024];

    ssize_t bytes = recvfrom(
        server, buf, sizeof(buf), 0,
        (struct sockaddr*)&caddr, &len
    );

    if (bytes < 0) {
        perror("Receive failed");
        close(server);
        return 1;
    }

    std::cout << "Client connected! Type messages below:\n";

    // 4. Read messages from the server console and send them to the client
    std::string msg;

    while (std::getline(std::cin, msg)) {
        // Send the message to the client using UDP
        if (sendto(
                server, msg.c_str(), msg.length(), 0,
                (struct sockaddr*)&caddr, len
            ) < 0) {
            perror("Message sending failed");
            break;
        }

        // Type /quit to end the chat
        if (msg == "/quit") {
            break;
        }
    }

    // 5. Close the socket
    close(server);

    return 0;
}
