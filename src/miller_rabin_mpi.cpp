#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "mod_exp.h"
#include "prime_utilities.h"

using namespace std;

// Miller-Rabin primality test (Single iteration per process)
bool miller_rabin_single_round(uint64_t n) {
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

    uint64_t a = random_in_range(2, n - 2);
    uint64_t x = mod_exp(a, d, n);

    if (x == 1 || x == n - 1) return true;

    for (int j = 0; j < r - 1; j++) {
        x = (x * x) % n;
        if (x == n - 1) return true;
    }

    return false; // Composite found
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint64_t num;
    int iterations = 5;  // Total rounds (can be increased)

    if (rank == 0) {
        cout << "Enter a number to test: ";
        cin >> num;
    }
    
    // Broadcast number to all processes
    MPI_Bcast(&num, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    // Each process performs one independent round of the Miller-Rabin test
    bool local_result = miller_rabin_single_round(num);

    // Gather results from all processes
    bool global_result;
    MPI_Reduce(&local_result, &global_result, 1, MPI_C_BOOL, MPI_LAND, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        if (global_result) {
            cout << num << " is probably prime.\n";
        } else {
            cout << num << " is composite.\n";
        }
    }

    MPI_Finalize();
    return 0;
}