# kev1n
A simulation of an autonomous robot using Raylib and Kev.

---

## What is this?
kev1n is an autonomous robot navigating around a 512x512 grid world. Its movement decisions come from __[Kev](https://github.com/jaredpalmer/kev)__, a small & non-generative model that returns typed decisions (such as direction & confidence) rather than generated text.

## Setup
Ensure that you have a [kev](https://github.com/jaredpalmer/kev) instance running locally at port ``8009`` 

<!--## Usage-->

---

## Building
### Requirements
* [CMake](https://cmake.org/) 3.23 or later
* MSVC 2022+

### Commands
```bash
cmake -B build/
cmake --build build/
```

---

## License
BSD-3-Clause, see [LICENSE](LICENSE)
