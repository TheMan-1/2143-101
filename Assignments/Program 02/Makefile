CXX := g++
CXXFLAGS := -Wall -std=c++17 -I/mingw64/include/SDL2 -DSDL_MAIN_HANDLED
LDFLAGS := -L/mingw64/lib -lSDL2
TARGET := Program_02
SRC := main.cpp
INCLUDES := -I./includes

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
