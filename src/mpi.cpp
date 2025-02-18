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
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    uint64_t start, end;
    int iterations = 100;

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
    MPI_Bcast(&start, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);
    MPI_Bcast(&end, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    // Compute subrange for this MPI rank
    uint64_t range_size = (end - start + 1);
    uint64_t chunk_size = range_size / size;
    uint64_t local_start = start + rank * chunk_size;
    uint64_t local_end = (rank == size - 1) ? end : local_start + chunk_size - 1;

    vector<uint64_t> local_primes;

    // OpenMP parallel execution
    #pragma omp parallel
    {
        vector<uint64_t> thread_primes;
        #pragma omp for nowait schedule(dynamic)
        for (uint64_t num = local_start; num <= local_end; num++) {
            if (miller_rabin(num, iterations)) {
                thread_primes.push_back(num);
            }
        }

        #pragma omp critical
        local_primes.insert(local_primes.end(), thread_primes.begin(), thread_primes.end());
    }

    // Only rank 0 will store the final prime numbers
    vector<uint64_t> global_primes;

    // DEBUG
    // cout << "DEBUG: Rank " << rank << " processing range [" << local_start << ", " << local_end << "]\n";
    
    // Ensure all ranks send their data **before** rank 0 prints
    if (rank == 0) {
        global_primes = local_primes;  // Store rank 0's own results

        // Receive results from other ranks
        for (int i = 1; i < size; i++) {
            int count;
            MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            vector<uint64_t> received_primes(count);
            MPI_Recv(received_primes.data(), count, MPI_UINT64_T, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            global_primes.insert(global_primes.end(), received_primes.begin(), received_primes.end());
        }

        // Sort and print final list (only in rank 0)
        sort(global_primes.begin(), global_primes.end());
        cout << "Prime numbers in range [" << start << ", " << end << "] are:\n";
        for (uint64_t prime : global_primes) {
            cout << prime << " ";
        }
        cout << endl;
    } else {
        // Other ranks send their results to rank 0
        int count = local_primes.size();
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_primes.data(), count, MPI_UINT64_T, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}