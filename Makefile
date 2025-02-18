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
SERIAL_EXEC = $(BIN_DIR)/serial
MPI_EXEC = $(BIN_DIR)/mpi

# Source Files
SERIAL_SRC = $(SRC_DIR)/serial.cpp $(SRC_DIR)/miller_rabin.cpp
MPI_SRC = $(SRC_DIR)/mpi.cpp $(SRC_DIR)/miller_rabin.cpp

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

# Compile MPI Object Files (Explicitly using mpicxx)
$(BUILD_DIR)/mpi.o: $(SRC_DIR)/mpi.cpp | $(BUILD_DIR)
	$(MPICXX) $(MPI_CXXFLAGS) -c $< -o $@

# Run Serial Version with Start and End arguments
run_serial: $(SERIAL_EXEC)
	$(SERIAL_EXEC) $(START) $(END)

# Run MPI Version with Start and End arguments
run_mpi: $(MPI_EXEC)
	$(MPI_EXEC) $(START) $(END)

# Clean compiled files
clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)

# Clean everything, including logs and benchmark results
clean-all: clean
	rm -f data/output.log data/benchmark_results.csv data/debug.log