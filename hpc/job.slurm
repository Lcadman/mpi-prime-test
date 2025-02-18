#!/bin/bash
#SBATCH --job-name="mpi"
#SBATCH --output="miller_rabin_output.txt"
#SBATCH --partition=peregrine-cpu      # Use CPU partition
#SBATCH --qos=cpu_debug                # Debug QoS, adjust as needed
#SBATCH --time=00:10:00                # Adjust runtime limit
#SBATCH --nodes=1                      # Single node for now
#SBATCH --ntasks=4                     # MPI processes (adjust as needed)
#SBATCH --cpus-per-task=2              # OpenMP threads per MPI task
#SBATCH --mem-per-cpu=1G               # Memory per CPU (adjust as needed)

# Load necessary modules
module purge
module load compilers/mpi/openmpi-slurm

# Export OpenMP threads count
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

# Navigate to project directory (update as needed)
cd $SLURM_SUBMIT_DIR

# Define the start and end values
START=1000000
END=1001000

# Run the MPI-OpenMP hybrid application
srun -n $SLURM_NTASKS --cpus-per-task=$SLURM_CPUS_PER_TASK /s/chopin/a/grad/lcadman/mpi-prime-test/bin/mpi $START $END