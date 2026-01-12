CXX = g++
CXXFLAGS = -Wall -O3 -std=c++11
LIBS = -lSDL2 -lm

SRCS = main.cpp raycaster.cpp
HEADERS = raycaster.h gameMap.h
TARGET = raycaster

all: $(TARGET)

$(TARGET): $(SRCS) $(HEADERS)
	$(CXX) -o $(TARGET) $(SRCS) $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

