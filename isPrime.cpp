#include <iostream>
#include <cmath>

using namespace std;

/*
Based on the fact that all primes are of the form 6k ± 1
*/
bool isPrime(unsigned int n) {
    if (n < 1) return false;
    if (n < 3) return true;
    for (int i = 5; i < sqrt(n); i += 6) {
        if (0 == n % i || 0 == n % (i + 2)) return false;
    }
    return true;
}

int main() {
    cout << isPrime(982451653);
    return 0;
}