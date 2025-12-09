#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <exception>
#include "../includes/ConwayLife.hpp"
#include "../includes/argsToJson.hpp"

using nlohmann::json;


class ConwayLifeInteractive : public ConwayLife {
public:
    using ConwayLife::ConwayLife;  // inherit constructor

    // Clear all cells to 0
    void clearAll() {
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                grid[r][c] = 0;
            }
        }
    }

    // Set a single cell (with bounds check)
    void setCell(int r, int c, int value) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return;
        grid[r][c] = value ? 1 : 0;
    }

    // Read a single cell (safe)
    int getCell(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return 0;
        return grid[r][c];
    }

    // Get the whole grid (for drawing)
    const std::vector<std::vector<int>>& getGridRef() const {
        return grid;
    }
};

// ------------------------------------------------------------
// Load a pattern by name from shapes.json into the center of
// the ConwayLifeInteractive grid.
// ------------------------------------------------------------
void loadPattern(const json& patternData,
                 ConwayLifeInteractive& gol,
                 const std::string& name) {
    if (patternData.is_null() || !patternData.contains("shapes")) {
        std::cerr << "Pattern data not loaded or missing 'shapes'.\n";
        return;
    }

    const auto& shapes = patternData["shapes"];
    if (!shapes.contains(name)) {
        std::cerr << "Pattern not found: " << name << "\n";
        return;
    }

    const auto& shape = shapes[name];

    const auto& gridRef = gol.getGridRef();
    int rows = static_cast<int>(gridRef.size());
    int cols = rows > 0 ? static_cast<int>(gridRef[0].size()) : 0;

    int centerR = rows / 2;
    int centerC = cols / 2;

    // Clear grid first
    gol.clearAll();

    // Place pattern cells relative to center
    for (const auto& cell : shape["cells"]) {
        int x = cell["x"]; // JSON uses x for column offset
        int y = cell["y"]; // JSON uses y for row offset

        int r = centerR + y;
        int c = centerC + x;

        gol.setCell(r, c, 1);
    }
}

int main(int argc, char* argv[]) {
    // ------------------------------------------------------------
    // Defaults & command-line params (use prof's ArgsToJson)
    // ------------------------------------------------------------
    json defaults = {
        {"window_width",  900},
        {"window_height", 900},
        {"cellSize",      20},
        {"frameDelayMs",  100}
    };

    json params = json::object();
    try {
        params = ArgsToJson(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Argument parse error: " << e.what() << "\n";
    }

    // merge defaults into params (only if key missing)
    for (auto& [key, value] : defaults.items()) {
        if (params.find(key) == params.end()) {
            params[key] = value;
        }
    }

    int windowWidth  = params["window_width"];
    int windowHeight = params["window_height"];
    int cellSize     = params["cellSize"];
    int frameDelayMs = params["frameDelayMs"];

    // How many cells fit in the window
    int cols = windowWidth  / cellSize;
    int rows = windowHeight / cellSize;

    std::cout << "Simulation Parameters:\n"
              << params.dump(4) << std::endl;

    // Seed RNG for ConwayLife::randomize()
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // ------------------------------------------------------------
    // Initialize SDL
    // ------------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // ------------------------------------------------------------
    // Create window
    // ------------------------------------------------------------
    SDL_Window* window = SDL_CreateWindow(
        "SDL_GOL_main - Conway (Using ConwayLife)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // ------------------------------------------------------------
    // Create renderer
    // ------------------------------------------------------------
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        std::cerr << "Renderer Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // ------------------------------------------------------------
    // Load shapes.json  (from ./assets)
    // ------------------------------------------------------------
    json patternData;
    {
        std::ifstream f("./assets/shapes.json");
        if (!f.is_open()) {
            std::cerr << "Could not open ./assets/shapes.json\n";
        } else {
            try {
                f >> patternData;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing shapes.json: " << e.what() << "\n";
            }
        }
    }

    // ------------------------------------------------------------
    // Create ConwayLife object (the REAL engine)
    // ------------------------------------------------------------
    ConwayLifeInteractive gol(rows, cols);

    // Start with a light random pattern (10% alive)
    gol.clearAll();
    gol.randomize(0.10);

    bool running = true;
    bool paused  = false;  // Space toggles this
    bool doStep  = false;  // N triggers a single step when paused
    SDL_Event event;

    while (running) {
        // -------------------- Events ----------------------------
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    running = false;
                    break;

                case SDLK_SPACE: // pause / resume
                    paused = !paused;
                    break;

                case SDLK_n: // step one generation (only when paused)
                    if (paused)
                        doStep = true;
                    break;

                case SDLK_c: // clear grid
                    gol.clearAll();
                    break;

                case SDLK_r: // randomize grid
                    gol.randomize(0.10);
                    break;

                // -------- Pattern hotkeys (1–9, 0) --------
                case SDLK_1:
                    loadPattern(patternData, gol, "glider");
                    break;
                case SDLK_2:
                    loadPattern(patternData, gol, "blinker");
                    break;
                case SDLK_3:
                    loadPattern(patternData, gol, "toad");
                    break;
                case SDLK_4:
                    loadPattern(patternData, gol, "beacon");
                    break;
                case SDLK_5:
                    loadPattern(patternData, gol, "lwss");
                    break;
                case SDLK_6:
                    loadPattern(patternData, gol, "r_pentomino");
                    break;
                case SDLK_7:
                    loadPattern(patternData, gol, "diehard");
                    break;
                case SDLK_8:
                    loadPattern(patternData, gol, "acorn");
                    break;
                case SDLK_9:
                    loadPattern(patternData, gol, "gosper_glider_gun");
                    break;
                case SDLK_0: // also clear
                    gol.clearAll();
                    break;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                int c = mouseX / cellSize;
                int r = mouseY / cellSize;

                int current = gol.getCell(r, c);
                gol.setCell(r, c, !current);
            }
        }

        // -------------------- Conway Life step ------------------
        if (!paused || doStep) {
            gol.step();       
            doStep = false;  
        }

        // -------------------- Clear background ------------------
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        // -------------------- Draw grid lines -------------------
        SDL_SetRenderDrawColor(renderer, 70, 70, 90, 255);

        // Vertical lines
        for (int c = 0; c <= cols; ++c) {
            int x = c * cellSize;
            SDL_RenderDrawLine(renderer, x, 0, x, windowHeight);
        }

        // Horizontal lines
        for (int r = 0; r <= rows; ++r) {
            int y = r * cellSize;
            SDL_RenderDrawLine(renderer, 0, y, windowWidth, y);
        }

        // -------------------- Draw live cells -------------------
        SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);

        const auto& grid = gol.getGridRef();
        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (grid[r][c] == 1) {
                    SDL_Rect cellRect;
                    cellRect.x = c * cellSize;
                    cellRect.y = r * cellSize;
                    cellRect.w = cellSize;
                    cellRect.h = cellSize;
                    SDL_RenderFillRect(renderer, &cellRect);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(frameDelayMs);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
