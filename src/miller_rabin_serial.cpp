#include <iostream>
#include <cstdlib>
#include <ctime>
#include "mod_exp.h"
#include "prime_utilities.h"

using namespace std;

// Miller-Rabin primality test
bool miller_rabin(uint64_t n, int k) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    // Write n-1 as 2^r * d
    uint64_t d = n - 1;
    int r = 0;
    while (d % 2 == 0) {
        d /= 2;
        r++;
    }

    for (int i = 0; i < k; i++) {
        uint64_t a = random_in_range(2, n - 2);
        uint64_t x = mod_exp(a, d, n);
        
        if (x == 1 || x == n - 1) continue;

        bool composite = true;
        for (int j = 0; j < r - 1; j++) {
            x = (x * x) % n;
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) return false;
    }
    return true;
}

int main() {
    srand(time(0));
    
    uint64_t num;
    cout << "Enter a number to test: ";
    cin >> num;
    
    int iterations = 5; // Number of rounds
    if (miller_rabin(num, iterations)) {
        cout << num << " is probably prime.\n";
    } else {
        cout << num << " is composite.\n";
    }
    
    return 0;
}