/**
 * ============================================================
 *  SDL2 Grid Example
 * ============================================================
 *  This program creates a simple window using SDL2 and
 *  renders a visible grid based on a given cell size.
 *
 *  Concepts introduced:
 *   - Converting grid coordinates to pixel coordinates
 *   - Drawing vertical & horizontal grid lines
 *   - Handling the basic SDL event loop
 *   - Using variables to control cell size, grid width, and height
 *
 */

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include "json.hpp"


#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// ------------------------------------------------------------
// Simple data structures for our pattern
// ------------------------------------------------------------
struct Cell {
    int x;
    int y;
};

struct Shape {
    string        name;
    int           width;
    int           height;
    vector<Cell>  cells;
};

struct Bounds {
    int min_x;
    int max_x;
    int min_y;
    int max_y;
};

// Compute bounding box of all cells (handles negative coords)
Bounds compute_bounds(const Shape& shape) {
    Bounds b{0, 0, 0, 0};

    if (shape.cells.empty()) {
        return b;
    }

    b.min_x = b.max_x = shape.cells[0].x;
    b.min_y = b.max_y = shape.cells[0].y;

    for (const auto& c : shape.cells) {
        if (c.x < b.min_x) b.min_x = c.x;
        if (c.x > b.max_x) b.max_x = c.x;
        if (c.y < b.min_y) b.min_y = c.y;
        if (c.y > b.max_y) b.max_y = c.y;
    }

    return b;
}

int main() {
    // ------------------------------------------------------------
    // CONFIGURATION SECTION
    // ------------------------------------------------------------
    // Each "cell" will be a square this many pixels wide/tall
    const int cellSize = 20;

    // Number of cells horizontally and vertically (used to size window)
    const int gridWidth  = 30;  // 30 cells across
    const int gridHeight = 20;  // 20 cells tall

    // Total pixel dimensions of the SDL window
    const int windowWidth  = cellSize * gridWidth;
    const int windowHeight = cellSize * gridHeight;

    // ------------------------------------------------------------
    // LOAD JSON PATTERN DATA
    // ------------------------------------------------------------
    ifstream f("shapes.json");   // rename to "patterns.json" if needed
    if (!f.is_open()) {
        cerr << "Error: could not open shapes.json\n";
        return 1;
    }

    json data;
    try {
        f >> data;
    } catch (const std::exception& e) {
        cerr << "JSON parse error: " << e.what() << "\n";
        return 1;
    }

    if (!data.contains("shapes")) {
        cerr << "Error: JSON missing 'shapes' key.\n";
        return 1;
    }

    json shapes_data = data["shapes"];

    // Pick which pattern to draw
    // Change this string or add input/argv logic if you want.
    string choice = "glider";

    if (!shapes_data.contains(choice)) {
        cerr << "Error: shape '" << choice << "' not found in JSON.\n";
        return 1;
    }

    auto  shape_json = shapes_data[choice];
    Shape shape;
    shape.name   = choice;
    shape.width  = shape_json["size"]["w"];
    shape.height = shape_json["size"]["h"];

    for (auto& cell : shape_json["cells"]) {
        Cell c;
        c.x = cell["x"];
        c.y = cell["y"];
        shape.cells.push_back(c);
    }

    // Compute bounding box and centering offsets
    Bounds b = compute_bounds(shape);

    int shapeWidthCells  = b.max_x - b.min_x + 1;
    int shapeHeightCells = b.max_y - b.min_y + 1;

    int patternPixelWidth  = shapeWidthCells * cellSize;
    int patternPixelHeight = shapeHeightCells * cellSize;

    int offsetX = (windowWidth  - patternPixelWidth)  / 2;
    int offsetY = (windowHeight - patternPixelHeight) / 2;

    // ------------------------------------------------------------
    // INITIALIZE SDL
    // ------------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return 1;  // Return non-zero to indicate an error
    }

    // ------------------------------------------------------------
    // CREATE THE WINDOW
    // ------------------------------------------------------------
    SDL_Window* window = SDL_CreateWindow(
        "Program 02 - SDL2 Shape Renderer",  // title
        SDL_WINDOWPOS_CENTERED,              // x position
        SDL_WINDOWPOS_CENTERED,              // y position
        windowWidth,                         // window width (pixels)
        windowHeight,                        // window height (pixels)
        SDL_WINDOW_SHOWN                     // flags
    );

    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // ------------------------------------------------------------
    // CREATE A RENDERER
    // ------------------------------------------------------------
    SDL_Renderer* renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ------------------------------------------------------------
    // RANDOM COLOR FOR PATTERN
    // ------------------------------------------------------------
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    Uint8 r = 50 + std::rand() % 206;
    Uint8 g = 50 + std::rand() % 206;
    Uint8 bcol = 50 + std::rand() % 206;

    // ------------------------------------------------------------
    // MAIN LOOP
    // ------------------------------------------------------------
    bool      running = true;
    SDL_Event event;

    while (running) {
        // --------------------------------------------------------
        // EVENT HANDLING
        // --------------------------------------------------------
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN &&
                       event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;  // ESC to quit
            }
        }

        // --------------------------------------------------------
        // CLEAR SCREEN (background)
        // --------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
        SDL_RenderClear(renderer);

        // --------------------------------------------------------
        // DRAW FILLED RECTANGLES FOR LIVE CELLS
        // --------------------------------------------------------
        SDL_SetRenderDrawColor(renderer, r, g, bcol, 255);

        for (const auto& c : shape.cells) {
            int gridX = c.x - b.min_x;  // shift so min_x -> 0
            int gridY = c.y - b.min_y;  // shift so min_y -> 0

            SDL_Rect rect;
            rect.x = offsetX + gridX * cellSize;
            rect.y = offsetY + gridY * cellSize;
            rect.w = cellSize;
            rect.h = cellSize;

            SDL_RenderFillRect(renderer, &rect);
        }

        // --------------------------------------------------------
        // SHOW THE RESULT
        // --------------------------------------------------------
        SDL_RenderPresent(renderer);

        // ~60 FPS
        SDL_Delay(16);
    }

    // ------------------------------------------------------------
    // CLEANUP
    // ------------------------------------------------------------
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
