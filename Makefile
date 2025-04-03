# Compiler setup
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I./include -g -static -Wno-missing-field-initializers -MMD -MP
LDFLAGS := -lkernel32 -luser32 -Wl,-subsystem,console

# Windows commands
SHELL := cmd
RM := del /q /f
MKDIR := mkdir

# Directory structure
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Targets
TARGET := $(BIN_DIR)\dispatcher.exe
WORKER_TARGET := $(BIN_DIR)\worker.exe

# Explicit source file listing (no wildcards)
ROOT_SOURCES := main.cpp worker_main.cpp
SRC_SOURCES := $(addprefix $(SRC_DIR)/, worker.cpp utils.cpp dispatcher.cpp config.cpp job.cpp) # Add src prefix to src files.

# Object files
ROOT_OBJS := $(ROOT_SOURCES:%.cpp=$(BUILD_DIR)/%.o)
SRC_OBJS := $(SRC_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
ALL_OBJS := $(ROOT_OBJS) $(SRC_OBJS)

# Main build targets
all: $(TARGET) $(WORKER_TARGET)

# Dispatcher needs all objects except worker_main.o
$(TARGET): $(filter-out $(BUILD_DIR)/worker_main.o, $(ALL_OBJS))
	@$(MKDIR) $(BIN_DIR) 2>nul ||:
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Worker needs specific objects
$(WORKER_TARGET): $(BUILD_DIR)/worker_main.o $(BUILD_DIR)/worker.o $(BUILD_DIR)/utils.o
	@$(MKDIR) $(BIN_DIR) 2>nul ||:
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Single unified pattern rule for all sources
$(BUILD_DIR)/%.o: %.cpp
	@$(MKDIR) $(BUILD_DIR) 2>nul ||:
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(BUILD_DIR) 2>nul ||:
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@$(RM) $(BUILD_DIR)\* $(BIN_DIR)\* 2>nul ||:

run: $(TARGET)
	@echo.
	@echo === DISPATCHER STARTED ===
	@echo Workers will launch in separate windows
	@echo Enter jobs in format: ^<type^> ^<duration^>
	@echo Press Ctrl+C to exit
	@echo.
	@$(TARGET)

.PHONY: all clean run

-include $(wildcard $(BUILD_DIR)/*.d)