# Project settings
CXX := g++
EXEC := menu
SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj

# Find all source files and convert to object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Compiler flags
CXXFLAGS := -Wall -std=c++17 -I$(INC_DIR)
LDFLAGS := -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the executable
run: $(EXEC)
	./$(EXEC)

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(EXEC)

.PHONY: all clean run
