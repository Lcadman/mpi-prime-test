#include "miller_rabin.h"
#include <random>
#include <vector>

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

// Fast Modular Exponentiation using Exponentiation by Squaring
uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp & 1) // If exp is odd, multiply base with result
            result = (result * base) % mod;
        exp >>= 1; // Right shift exp by 1 (divide by 2)
        base = (base * base) % mod; // Square the base
    }
    
    return result;
}

// Generate a random number in range [low, high] using C++11 random
uint64_t random_in_range(uint64_t low, uint64_t high) {
    static std::random_device rd;  // Obtain a random seed
    static std::mt19937_64 gen(rd()); // 64-bit Mersenne Twister RNG
    std::uniform_int_distribution<uint64_t> dist(low, high);
    return dist(gen);
}

// Return a list of small primes useful for deterministic primality tests
std::vector<uint64_t> get_small_prime_bases(uint64_t n) {
    // These small primes are commonly used for primality testing
    static const std::vector<uint64_t> small_primes = {
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47
    };
    
    std::vector<uint64_t> bases;
    for (auto prime : small_primes) {
        if (prime >= n) break; // Only return bases smaller than n
        bases.push_back(prime);
    }
    return bases;
}