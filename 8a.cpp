#include <iostream>
using namespace std;

int main()
{
    int bucketSize, outputRate, n, packet, stored = 0;

    cout << "Enter bucket size: ";
    cin >> bucketSize;

    cout << "Enter output rate: ";
    cin >> outputRate;

    cout << "Enter number of packets: ";
    cin >> n;

    for (int i = 1; i <= n; i++)
    {
        cout << "\nEnter packet size: ";
        cin >> packet;

        if (stored + packet <= bucketSize)
        {
            stored += packet;
            cout << "Packet accepted";
        }
        else
        {
            cout << "Bucket overflow! Packet dropped";
        }

        cout << "\nPackets transmitted: ";

        int sent = min(stored, outputRate);
        stored -= sent;

        cout << sent;
        cout << "\nPackets remaining in bucket: " << stored << endl;
    }

    return 0;
}
