#include <iostream>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

using namespace std;

// Compile and run the client in Terminal 2:
// g++ client.cpp -o client
// ./client

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

        // Stop when the null character is received
        if (ch == '\0')
            return true;

        msg += ch;
    }
}

int main()
{
    int s;
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
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 3. Connect to the server
    if (connect(s, (sockaddr*)&server, sizeof(server)) < 0)
    {
        perror("Connection failed");
        close(s);
        return 1;
    }

    // 4. Enter the name of the file to be requested
    cout << "Enter file name: ";
    cin >> filename;

    // 5. Send the filename to the server
    if (!sendMessage(s, filename))
    {
        cerr << "Failed to send filename." << endl;
        close(s);
        return 1;
    }

    cout << "\nFile contents:\n";

    // 6. Receive and display file contents from the server
    while (true)
    {
        if (!receiveMessage(s, buffer))
        {
            cerr << "Connection closed or receive failed." << endl;
            break;
        }

        // Stop when the server sends END
        if (buffer == "END")
            break;

        // Display the error if the file does not exist
        if (buffer == "File not found")
        {
            cout << buffer << endl;
            break;
        }

        // Display the received line
        cout << buffer << endl;
    }

    // 7. Close the client socket
    close(s);

    return 0;
}
