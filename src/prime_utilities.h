#ifndef PRIME_UTILITIES_H
#define PRIME_UTILITIES_H

#include <cstdint>
#include <vector>

// Generate a random number in range [low, high]
uint64_t random_in_range(uint64_t low, uint64_t high);

// Get a list of small prime bases for Miller-Rabin deterministic tests
std::vector<uint64_t> get_small_prime_bases(uint64_t n);

#endif // PRIME_UTILITIES_H