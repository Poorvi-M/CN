#include <iostream>
using namespace std;

int gcd(int a, int b)
{
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

int power(int base, int exp, int mod)
{
    int result = 1;

    for (int i = 0; i < exp; i++)
        result = (result * base) % mod;

    return result;
}

int main()
{
    int p = 3, q = 11;
    int n = p * q;
    int phi = (p - 1) * (q - 1);

    int e = 7, d = 1;

    // Find public key exponent e
    while (gcd(e, phi) != 1)
        e++;

    // Find private key exponent d
    while ((d * e) % phi != 1)
        d++;

    int msg;
    cout << "Enter message (number): ";
    cin >> msg;

    // Encryption
    int encrypted = power(msg, e, n);
    cout << "Encrypted message: " << encrypted << endl;

    // Decryption
    int decrypted = power(encrypted, d, n);
    cout << "Decrypted message: " << decrypted << endl;

    cout << "\nPublic Key: (" << e << ", " << n << ")";
    cout << "\nPrivate Key: (" << d << ", " << n << ")" << endl;

    return 0;
}
