#include <iostream>
#include <cstdlib>
using namespace std;

int main()
{
    int maxQueue, n, packet;
    float minThreshold, maxThreshold;
    float avgQueue = 0, maxProbability, probability;
    int queue = 0;

    cout << "Enter maximum queue size: ";
    cin >> maxQueue;

    cout << "Enter minimum threshold: ";
    cin >> minThreshold;

    cout << "Enter maximum threshold: ";
    cin >> maxThreshold;

    cout << "Enter maximum drop probability: ";
    cin >> maxProbability;

    cout << "Enter number of packets: ";
    cin >> n;

    for (int i = 1; i <= n; i++)
    {
        cout << "\nEnter packet size: ";
        cin >> packet;

        avgQueue = (avgQueue + queue) / 2;

        if (avgQueue < minThreshold)
        {
            queue += packet;
            cout << "Packet accepted";
        }
        else if (avgQueue >= maxThreshold)
        {
            cout << "Packet dropped";
        }
        else
        {
            probability = maxProbability *
                (avgQueue - minThreshold) /
                (maxThreshold - minThreshold);

            float randomValue = (float)rand() / RAND_MAX;

            if (randomValue < probability)
            {
                cout << "Packet dropped randomly";
            }
            else
            {
                queue += packet;
                cout << "Packet accepted";
            }
        }

        if (queue > maxQueue)
        {
            queue = maxQueue;
            cout << "\nQueue is full";
        }

        cout << "\nCurrent queue size: " << queue << endl;

        // Simulate packet transmission
        if (queue > 0)
            queue--;
    }

    return 0;
}
