#include <iostream>
#include <cmath>

using namespace std;

/*
Based on the fact that all primes are of the form 6k ± 1
*/
bool isPrime(unsigned int n) {
    if (n < 2) return false; // 1 is not prime
    if (n < 4) return true; // 2,3 are prime numbers
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int i = 5; i < sqrt(n); i += 6) {
        if (0 == n % i || 0 == n % (i + 2)) return false;
    }
    return true;
}

int main() {

    for (int i = 0; i < 100; ++i) {
        if (isPrime(i)) {
            cout << i << endl;
        }
    }
    return 0;
}