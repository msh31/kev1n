# kev1n
A robot simulation where a decision-model picks the moves around a grid

---

## What is this?
kev1n is a simulation using __[Kev](https://github.com/jaredpalmer/kev)__ (A small & non-generative model that returns typed decisions (such as direction & confidence) rather than generated text)  

The model is handed a set of allowed directions and a small grid of its surroundings. Based on those, kev picks the robot's next move.

## Setup
Ensure that you have a [kev](https://github.com/jaredpalmer/kev) instance running locally at port ``8009`` 

<!--## Usage-->

## Building
### Requirements
* [CMake](https://cmake.org/) 3.23 or later
* A C++23-capable compiler (GCC 15+, Clang 16+, MSVC 2022+)

### Commands
```bash
cmake -B build/
cmake --build build/
```

---

## License
BSD-3-Clause, see [LICENSE](LICENSE)
