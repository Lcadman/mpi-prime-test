# Compilers
CXX = g++          # Serial and general compilation
MPICXX = mpicxx    # MPI Compiler (Automatically links MPI)
CC = gcc

# Compiler Flags
CXXFLAGS = -O2 -Wall -Wextra -std=c++17 -fopenmp  # Enable OpenMP
MPI_CXXFLAGS = $(CXXFLAGS)                         # MPI also needs OpenMP

# Directories
SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build

# Output Executables
SERIAL_EXEC = $(BIN_DIR)/miller_rabin_serial
MPI_EXEC = $(BIN_DIR)/miller_rabin_mpi

# Source Files
SERIAL_SRC = $(SRC_DIR)/miller_rabin_serial.cpp $(SRC_DIR)/mod_exp.cpp $(SRC_DIR)/prime_utilities.cpp
MPI_SRC = $(SRC_DIR)/miller_rabin_mpi.cpp $(SRC_DIR)/mod_exp.cpp $(SRC_DIR)/prime_utilities.cpp

# Object Files
SERIAL_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SERIAL_SRC))
MPI_OBJ = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(MPI_SRC))

# Default Target: Build Everything
all: $(SERIAL_EXEC) $(MPI_EXEC)

# Ensure directories exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build Serial Version
$(SERIAL_EXEC): $(SERIAL_OBJ) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build MPI Version (Using mpicxx with OpenMP support)
$(MPI_EXEC): $(MPI_OBJ) | $(BIN_DIR)
	$(MPICXX) $(MPI_CXXFLAGS) -o $@ $^

# Compile Object Files (Serial)
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile Object Files (MPI)
$(BUILD_DIR)/miller_rabin_mpi.o: $(SRC_DIR)/miller_rabin_mpi.cpp | $(BUILD_DIR)
	$(MPICXX) $(MPI_CXXFLAGS) -c $< -o $@

# Run MPI version locally
run_mpi: $(MPI_EXEC)
	mpirun -np 4 --bind-to none $(MPI_EXEC)

# Run Serial version
run_serial: $(SERIAL_EXEC)
	$(SERIAL_EXEC)

# Clean compiled files
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

# Clean everything, including logs and benchmark results
clean-all: clean
	rm -f data/output.log data/benchmark_results.csv data/debug.log