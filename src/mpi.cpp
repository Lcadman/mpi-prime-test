#include <mpi.h>
#include <omp.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "miller_rabin.h"

using namespace std;

int main(int argc, char* argv[]) {
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    // Get rank and size
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the current MPI process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get total number of MPI processes

    uint64_t start, end; // Range of numbers to check for primality
    int iterations = 100; // Number of Miller-Rabin iterations

    // Rank 0 reads command-line arguments and broadcasts them
    if (rank == 0) {
        if (argc != 3) {
            cerr << "Usage: " << argv[0] << " <start> <end>\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        start = strtoull(argv[1], nullptr, 10);
        end = strtoull(argv[2], nullptr, 10);
    }

    // Broadcast start and end values to all MPI ranks
    MPI_Bcast(&start, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD); // Broadcast start value
    MPI_Bcast(&end, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD); // Broadcast end value

    // Compute subrange for this MPI rank
    uint64_t range_size = (end - start + 1); // Inclusive range
    uint64_t chunk_size = range_size / size; // Number of numbers to check per rank
    uint64_t local_start = start + rank * chunk_size; // Start of local range
    uint64_t local_end = (rank == size - 1) ? end : local_start + chunk_size - 1; // End of local range

    vector<uint64_t> local_primes; // Local prime numbers found by this rank

    // OpenMP parallel execution
    #pragma omp parallel // Each thread will have its own vector of prime numbers
    {
        vector<uint64_t> thread_primes; // Prime numbers found by this thread
        #pragma omp for nowait schedule(dynamic) // Distribute iterations dynamically
        for (uint64_t num = local_start; num <= local_end; num++) { // Check each number in the range
            if (miller_rabin(num, iterations)) { // If the number is prime
                thread_primes.push_back(num); // Add it to the list of prime numbers
            }
        }

        #pragma omp critical // Only one thread at a time can access the shared vector
        local_primes.insert(local_primes.end(), thread_primes.begin(), thread_primes.end()); // Add thread's primes to rank's primes
    }

    // Only rank 0 will store the final prime numbers
    vector<uint64_t> global_primes; // Final list of prime numbers

    // DEBUG
    // cout << "DEBUG: Rank " << rank << " processing range [" << local_start << ", " << local_end << "]\n";
    
    // Ensure all ranks send their data **before** rank 0 prints
    if (rank == 0) { 
        global_primes = local_primes;  // Store rank 0's own results

        // Receive results from other ranks
        for (int i = 1; i < size; i++) { // Start from rank 1
            int count; // Number of primes received from rank i
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive count
            vector<uint64_t> received_primes(count); // Buffer to store received primes
            MPI_Recv(received_primes.data(), count, MPI_UINT64_T, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE); // Receive primes
            global_primes.insert(global_primes.end(), received_primes.begin(), received_primes.end()); // Add received primes to global list
        }

        // Sort and print final list (only in rank 0)
        sort(global_primes.begin(), global_primes.end()); // Sort the list of prime numbers
        cout << "Prime numbers in range [" << start << ", " << end << "] are:\n"; // Print the prime numbers
        for (uint64_t prime : global_primes) { // Print each prime number
            cout << prime << " "; 
        }
        cout << endl;
    } else {
        // Other ranks send their results to rank 0
        int count = local_primes.size(); // Number of primes found by this rank
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); // Send count
        MPI_Send(local_primes.data(), count, MPI_UINT64_T, 0, 1, MPI_COMM_WORLD); // Send primes
    }

    MPI_Finalize(); // Finalize MPI
    return 0;
}