#include <iostream>
#include <cstdlib>
#include <ctime>
#include "mod_exp.h"
#include "prime_utilities.h"

using namespace std;

// Miller-Rabin Primality Test
bool miller_rabin(uint64_t n, int k) {
    // Step 1: Handle small numbers and even cases
    if (n < 2) return false;       // Numbers < 2 are not prime
    if (n == 2 || n == 3) return true;  // 2 and 3 are prime numbers
    if (n % 2 == 0) return false;  // Any even number > 2 is not prime

    // Step 2: Express (n - 1) as 2^r * d
    uint64_t d = n - 1;
    int r = 0;
    while (d % 2 == 0) {  // Factor out powers of 2 from (n-1)
        d /= 2;
        r++;
    }
    // Now, n - 1 = 2^r * d, where d is odd.

    // Step 3: Perform k rounds of testing
    for (int i = 0; i < k; i++) {
        uint64_t a = random_in_range(2, n - 2); // Choose a random base 'a' such that 2 ≤ a ≤ n-2
        uint64_t x = mod_exp(a, d, n);  // Compute x = a^d % n

        // Step 4: Check the first condition
        if (x == 1 || x == n - 1) continue;  
        // If x == 1 or x == n-1, then n is **probably** prime for this base.

        // Step 5: Perform r-1 squaring steps to check x^(2^j) % n
        bool composite = true;
        for (int j = 0; j < r - 1; j++) {
            x = (x * x) % n; // Square x (x^(2^j) mod n)

            if (x == n - 1) {  
                // If x ever becomes n-1, then n is **probably prime** for this base.
                composite = false;
                break;
            }
        }
        
        // If we never found x = n-1, then we found a witness for compositeness.
        if (composite) return false;
    }

    // If we survived k rounds, n is **probably** prime.
    return true;
}

int main() {
    srand(time(0)); // Seed the random number generator for random base selection

    uint64_t num;
    cout << "Enter a number to test for primality: ";
    cin >> num;

    int iterations = 5; // Number of rounds for Miller-Rabin test
    bool is_prime = miller_rabin(num, iterations);

    // Output result
    if (is_prime) {
        cout << num << " is probably prime.\n";
    } else {
        cout << num << " is composite.\n";
    }

    return 0;
}