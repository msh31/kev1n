# kev1n
A robot simulation where a decision-model picks the moves around a grid

---

## What is this?
kev1n is a simulation using __[Kev](https://github.com/jaredpalmer/kev)__ (A small & non-generative model that returns typed decisions (such as direction & confidence) rather than generated text)  

The model is handed a set of allowed directions and a small grid of its surroundings. Based on those, kev picks the robot's next move.

## Building
### Requirements
* [CMake](https://cmake.org/) 3.23 or later
* A C++23-capable compiler (GCC 15+, Clang 16+, MSVC 2022+)
* [Emscripten](https://emscripten.org/)
* [Ninja](https://ninja-build.org/)

### Commands
```bash
emcmake cmake -B build-web -G Ninja
cmake --build build-web/
# use a python server to serve or sum
```

---

## License
BSD-3-Clause, see [LICENSE](LICENSE)
