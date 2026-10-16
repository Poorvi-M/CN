#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

// Compile and run the server in Terminal 1:
// g++ server.cpp -o server
// ./server

// Send a complete null-terminated message over TCP
bool sendMessage(int sock, const string& msg)
{
    const char* data = msg.c_str();
    size_t remaining = msg.length() + 1;

    while (remaining > 0)
    {
        ssize_t sent = send(sock, data, remaining, 0);

        if (sent <= 0)
            return false;

        data += sent;
        remaining -= sent;
    }

    return true;
}

// Receive one complete null-terminated message over TCP
bool receiveMessage(int sock, string& msg)
{
    msg.clear();
    char ch;

    while (true)
    {
        ssize_t bytes = recv(sock, &ch, 1, 0);

        if (bytes <= 0)
            return false;

        // Null character marks the end of the message
        if (ch == '\0')
            return true;

        msg += ch;
    }
}

int main()
{
    int s, c;
    string filename, buffer;

    // 1. Create a TCP socket
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        perror("Socket creation failed");
        return 1;
    }

    // 2. Configure the server address and port
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;

    // 3. Bind the socket to port 8080
    if (bind(s, (sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("Bind failed");
        close(s);
        return 1;
    }

    // 4. Listen for incoming client connections
    if (listen(s, 5) < 0)
    {
        perror("Listen failed");
        close(s);
        return 1;
    }

    cout << "Waiting for client..." << endl;

    // 5. Accept a client connection
    c = accept(s, nullptr, nullptr);

    if (c < 0)
    {
        perror("Accept failed");
        close(s);
        return 1;
    }

    // 6. Receive the requested filename from the client
    if (!receiveMessage(c, filename))
    {
        cerr << "Failed to receive filename." << endl;
        close(c);
        close(s);
        return 1;
    }

    // 7. Open the requested file
    ifstream file(filename);

    if (!file)
    {
        // If the file does not exist, inform the client
        sendMessage(c, "File not found");
    }
    else
    {
        // 8. Read and send the file one line at a time
        while (getline(file, buffer))
        {
            if (!sendMessage(c, buffer))
            {
                cerr << "Failed to send file contents." << endl;
                break;
            }
        }

        // 9. Send END to indicate that file transfer is complete
        sendMessage(c, "END");

        file.close();
    }

    // 10. Close the client and server sockets
    close(c);
    close(s);

    return 0;
}
