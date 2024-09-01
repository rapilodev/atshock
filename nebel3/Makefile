# Makefile for compiling an SDL application

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -I../include -std=c++11

# SDL2 library
SDL2_LIB = -lSDL2

# Target executable
TARGET = water

# Source files
SRCS = ../include/mcga.cpp nebel.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(SDL2_LIB)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
