#include <iostream>
#include <string>
#include <iomanip>
using namespace std;

unsigned short crc_ccitt(string data)
{
    unsigned short crc = 0xFFFF;

    for (char ch : data)
    {
        crc ^= ((unsigned short)ch << 8);

        for (int i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    return crc;
}

int main()
{
    string data;

    cout << "Enter data: ";
    cin >> data;

    unsigned short crc = crc_ccitt(data);

    cout << "CRC-CCITT (16-bit): "
         << hex << uppercase
         << setw(4) << setfill('0')
         << crc << endl;

    cout << "Transmitted data: "
         << data << hex << uppercase
         << setw(4) << setfill('0')
         << crc << endl;

    return 0;
}
