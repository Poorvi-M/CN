#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

// Structure to represent a network frame
struct Frame {
    int seqNo;
    string data;
};

int main() {

    string message;
    int chunkSize;
    vector<Frame> frames;

    // Input message
    cout << "Enter the message to be transmitted: ";
    getline(cin, message);

    // Input frame size
    cout << "Enter the frame chunk size: ";
    cin >> chunkSize;

    // 1. Divide the message into frames
    int msgLen = message.length();
    int seq = 0;

    for (int i = 0; i < msgLen; i += chunkSize) {

        Frame f;

        // Assign sequence number
        f.seqNo = seq++;

        // Extract chunk of message
        f.data = message.substr(i, chunkSize);

        // Store frame
        frames.push_back(f);
    }

    // 2. Shuffle frames randomly
    // to simulate out-of-order arrival

    srand(static_cast<unsigned int>(time(0)));

    for (size_t i = frames.size() - 1; i > 0; --i) {

        size_t j = rand() % (i + 1);

        swap(frames[i], frames[j]);
    }

    // Display received frames
    cout << "\n--- Frames Received Out of Order at Receiver ---"
         << endl;

    cout << "Seq No\tData" << endl;

    for (const auto& f : frames) {

        cout << f.seqNo << "\t"
             << f.data << endl;
    }

    // 3. Sort frames using Bubble Sort

    int n = frames.size();

    for (int i = 0; i < n - 1; ++i) {

        for (int j = 0; j < n - i - 1; ++j) {

            if (frames[j].seqNo > frames[j + 1].seqNo) {

                swap(frames[j], frames[j + 1]);
            }
        }
    }

    // Display sorted frames
    cout << "\n--- Frames After Applying Sorting Technique ---"
         << endl;

    cout << "Seq No\tData" << endl;

    for (const auto& f : frames) {

        cout << f.seqNo << "\t"
             << f.data << endl;
    }

    // 4. Reconstruct original message

    cout << "\nReconstructed Message: ";

    for (const auto& f : frames) {

        cout << f.data;
    }

    cout << endl;

    return 0;
}
