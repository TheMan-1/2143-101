CXX = g++
CXXFLAGS = -std=c++17 -I./includes -I/mingw64/include/SDL2
LDFLAGS = -L/mingw64/lib -lmingw32 -lSDL2main -lSDL2

SRC = src/SDL_GOL_main.cpp
EXE = SDL_GOL_main.exe

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE) $(LDFLAGS)

clean:
	rm -f $(EXE)
