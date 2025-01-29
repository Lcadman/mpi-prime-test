#include "mod_exp.h"

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