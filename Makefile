# Pointer Pool
# Build and run with:  make run
# (Run it from this folder: the font is loaded from assets/)

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib)

SRC = src/main.cpp src/play.cpp src/pool.cpp src/questions.cpp src/levels.cpp \
      src/editor.cpp src/ui.cpp src/lab1.cpp src/lab2.cpp
HEADERS = src/app.h src/pool.h src/questions.h src/editor.h src/ui.h src/lab1.h src/lab2.h
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
