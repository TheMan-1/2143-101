# **Program 03–04**

### **Gage Woodall**

## **Description:**

Turns the Game of Life into a full **interactive SDL2 simulator**. Loads ConwayLife shapes from a JSON file, draws them inside an SDL2 window, and lets the user interact with the simulation using the mouse and keyboard.

This version supports:

* Clicking cells on/off
* Pausing the simulation
* Stepping one generation at a time
* Randomizing the grid
* Clearing the board
* Loading classic Life patterns (glider, blinker, gun, etc.)
* Command-line configuration (window size, cell size, speed)

---

|  #  | File                      | Description                                                     |
| :-: | ------------------------- | --------------------------------------------------------------- |
|  1  | **SDL_GOL_main.cpp**      | Main SDL2 program (rendering, input, simulation loop, patterns) |
|  2  | **shapes.json**           | Contains all Life patterns (glider, gun, r-pentomino, etc.)     |
|  3  | **ConwayLife.hpp**        | Professor’s Game of Life logic                                  |
|  4  | **CellularAutomaton.hpp** | Base automaton class                                            |
|  5  | **AutomatonUtils.hpp**    | Helper functions                                                |
|  6  | **argsToJson.hpp**        |                |
|  7  | **json.hpp**              |JSON                                         |
|  8  | **Screen.hpp**            | Provided screen class                         |
|  9  | **click.cpp**             | Clicking example file                           |
|  10 | **Makefile**              | Builds and runs the whole project                               |



---

## **Instructions**

* Keep all files together in the same folder (`Program_03-04`)
* Open **MSYS2 MinGW64**
* Navigate to the folder:

```
cd "Program_03-04"
```

* Build and run:

```
make run
```

Or run with custom settings:

```
./SDL_GOL_main window_width=900 window_height=900 cellSize=20 frameDelayMs=80
```

---

## **Controls**

* **Space** — Pause / Resume
* **N** — Step one generation
* **R** — Randomize grid
* **C** — Clear grid
* **1–9** — Load patterns
* **Mouse Click** — Toggle cell on/off
* **Q / ESC** — Quit
* **And 1-9 toggles shapes

---
