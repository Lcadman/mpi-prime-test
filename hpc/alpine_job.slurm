#!/bin/bash
#SBATCH --job-name="miller_rabin_mpi"
#SBATCH --output="miller_rabin_output_alpine.txt"
#SBATCH --partition=amilan               # Adjust for CU Alpine partition
#SBATCH --qos=normal                     # Use normal QoS unless debugging
#SBATCH --time=00:10:00                  # Adjust runtime as needed
#SBATCH --nodes=1                        # Single node (modify if needed)
#SBATCH --ntasks=4                       # MPI processes (adjust as needed)
#SBATCH --cpus-per-task=2                # OpenMP threads per MPI task
#SBATCH --mem-per-cpu=2G                 # More memory for Alpine

# Load necessary modules for CU Alpine
module purge
module load gcc/11.2.0 openmpi/4.1.4      # Adjust versions as needed

# Set OpenMP thread count
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Navigate to project directory (update if needed)
cd $SLURM_SUBMIT_DIR

# Define the start and end values
START=0
END=1000000

# Run the MPI-OpenMP hybrid program
srun --mpi=pmix mpirun -np $SLURM_NTASKS --bind-to none bin/miller_rabin_mpi $START $END