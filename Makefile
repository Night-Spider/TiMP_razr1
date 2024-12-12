CXX = g++
CXXFLAGS = -std=c++11 -Wall -I./ -I/usr/include/crypto++
LDFLAGS = -lcryptopp -lpthread
SRC = main.cpp base.cpp calc.cpp communicator.cpp interface.cpp log.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
