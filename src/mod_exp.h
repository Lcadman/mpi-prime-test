#ifndef MOD_EXP_H
#define MOD_EXP_H

#include <cstdint>  // For fixed-width integer types

// Function for modular exponentiation: (base^exp) % mod
uint64_t mod_exp(uint64_t base, uint64_t exp, uint64_t mod);

#endif // MOD_EXP_H