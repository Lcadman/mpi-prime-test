#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
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

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint64_t start, end;
    int iterations = 100;  // Number of rounds for Miller-Rabin test

    if (rank == 0) {
        cout << "Enter the start of the range: ";
        cin >> start;
        cout << "Enter the end of the range: ";
        cin >> end;
    }

    // Broadcast the range to all processes
    MPI_Bcast(&start, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&end, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    // Divide the range among MPI processes
    uint64_t range_size = (end - start + 1);
    uint64_t chunk_size = range_size / size;
    uint64_t local_start = start + rank * chunk_size;
    uint64_t local_end = (rank == size - 1) ? end : local_start + chunk_size - 1;

    // Store local primes
    vector<uint64_t> local_primes;

    // Parallel execution using OpenMP
    #pragma omp parallel
    {
        vector<uint64_t> thread_primes;
        #pragma omp for nowait schedule(dynamic)
        for (uint64_t num = local_start; num <= local_end; num++) {
            if (miller_rabin(num, iterations)) {
                thread_primes.push_back(num);
            }
        }

        // Merge thread results into local_primes
        #pragma omp critical
        local_primes.insert(local_primes.end(), thread_primes.begin(), thread_primes.end());
    }

    // Gather results from all processes at rank 0
    if (rank == 0) {
        vector<uint64_t> global_primes;

        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<uint64_t> received_primes(count);
            MPI_Recv(received_primes.data(), count, MPI_UINT64_T, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_primes.insert(global_primes.end(), received_primes.begin(), received_primes.end());
        }

        // Merge rank 0's own primes
        global_primes.insert(global_primes.end(), local_primes.begin(), local_primes.end());

        // Print results
        cout << "Prime numbers in range [" << start << ", " << end << "] are:\n";
        for (uint64_t prime : global_primes) {
            cout << prime << " ";
        }
        cout << endl;
    } else {
        int count = local_primes.size();
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_primes.data(), count, MPI_UINT64_T, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}