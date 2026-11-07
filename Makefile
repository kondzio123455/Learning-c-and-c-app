# Pointer Pool
# Build and run with:  make run

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib)

SRC = src/main.cpp src/table.cpp src/level.cpp src/lab2.cpp
HEADERS = src/table.h src/level.h src/lab2.h
TARGET = pointer_pool

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf $(TARGET).dSYM

.PHONY: all run clean
