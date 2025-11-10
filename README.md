# Metamod-FWGS

This is fork of [Metamod-R](https://github.com/rehlds/metamod-r) for use with Xash3D FWGS engine on many different architectures & platforms.

Original Metamod-R contains a lot of x86 JIT optimizations, patching hacks, constrained by binary compatibility with ReHLDS and legacy game libraries, and not flexible enough to be compatible with wide variety of compilers/toolchains and architectures.

## Documentation

* [Actual documentation](https://github.com/rehlds/metamod-r/wiki) (![en](https://i.imgur.com/rm67tUZ.png) **English** and ![ru](https://i.imgur.com/ItziiKg.png) **Russian**)
* ![en](https://i.imgur.com/rm67tUZ.png) Actual [list of supported games](https://github.com/rehlds/metamod-r/wiki/Supported-games)
* ![ru](https://i.imgur.com/ItziiKg.png) Актуальный [список поддерживаемых игр](https://github.com/rehlds/metamod-r/wiki/Поддерживаемые-игры)

## Build instructions
### Windows

#### Prerequisites
Visual Studio 2015 (C++14 standard) and later. You can use Visual Studio 2019 or Visual Studio 2022 for best experience.

#### Instructions
* Open cloned repository directory as CMake folder with Visual Studio (you can use VS2019 or VS2022).
* Select desired build preset, for example you can use `Windows / x86 / Debug`. You can see other available presets in CMakePresets.json file.
* In Build menu select Build solution, or you can use hotkey `Ctrl+Shift+B` instead.

### Linux

#### Prerequisites
`git`, `cmake`, `ninja-build`, `gcc` or `clang` (with C++17 support)

#### Instructions

```
cmake -E make_directory ./build
cd build
cmake .. --preset linux-x86-debug
cmake --build .
```
