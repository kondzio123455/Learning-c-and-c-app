# Pointer Pool
# Build and run with:  make run
# (Run it from this folder: the font is loaded from assets/)

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -g $(shell pkg-config --cflags raylib)
LIBS = $(shell pkg-config --libs raylib)

SRC = src/main.cpp src/play.cpp src/pool.cpp src/effects.cpp src/ui.cpp src/editor.cpp \
      src/questions.cpp src/lessons_pointers.cpp src/lessons_memory.cpp src/lessons_lists.cpp \
      src/lab1.cpp src/lab2.cpp
HEADERS = src/app.h src/pool.h src/effects.h src/ui.h src/editor.h src/questions.h src/lab1.h src/lab2.h
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
