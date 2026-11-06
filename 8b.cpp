#include <iostream>
using namespace std;

int main()
{
    int bucketSize, tokenRate, n, packet;
    int tokens = 0;

    cout << "Enter bucket size: ";
    cin >> bucketSize;

    cout << "Enter token generation rate: ";
    cin >> tokenRate;

    cout << "Enter number of packets: ";
    cin >> n;

    for (int i = 1; i <= n; i++)
    {
        tokens += tokenRate;

        if (tokens > bucketSize)
            tokens = bucketSize;

        cout << "\nEnter packet size: ";
        cin >> packet;

        if (packet <= tokens)
        {
            tokens -= packet;
            cout << "Packet transmitted successfully";
        }
        else
        {
            cout << "Not enough tokens! Packet discarded";
        }

        cout << "\nRemaining tokens: " << tokens << endl;
    }

    return 0;
}
