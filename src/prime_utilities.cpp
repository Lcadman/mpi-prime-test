#include "prime_utilities.h"
#include <random>
#include <vector>

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