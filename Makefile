CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude `pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5`
LDFLAGS = `pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5`

SRC = src/main.cpp src/Menu.cpp
OUT = menu

all:
	$(CXX) $(SRC) -o $(OUT) $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OUT)
