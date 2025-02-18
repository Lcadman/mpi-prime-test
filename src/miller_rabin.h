#ifndef MOD_EXP_H
#define MOD_EXP_H

#include <cstdint>
#include <cstdint>
#include <vector>

// Miller-Rabin Primality Test
bool miller_rabin(uint64_t n, int k);

// Function for modular exponentiation: (base^exp) % mod
uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod);

// Generate a random number in range [low, high]
uint64_t random_in_range(uint64_t low, uint64_t high);

// Get a list of small prime bases for Miller-Rabin deterministic tests
std::vector<uint64_t> get_small_prime_bases(uint64_t n);

#endif // MOD_EXP_H