# Metamod-FWGS

Fork of [Metamod-R](https://github.com/rehlds/metamod-r) for use with Xash3D FWGS engine on many different architectures. Original Metamod-R contains a lot of x86-only JIT optimizations, constrained by binary compatibility with ReHLDS and game libraries, and not flexible enough to be compatible with wide variety of compilers/toolchains and architectures.

## Documentation

* [Actual documentation](https://github.com/rehlds/metamod-r/wiki) in ![en](https://i.imgur.com/rm67tUZ.png) **English** and ![ru](https://i.imgur.com/ItziiKg.png) **Russian** languages
* ![en](https://i.imgur.com/rm67tUZ.png) Actual [list of supported games](https://github.com/rehlds/metamod-r/wiki/Supported-games)
* ![ru](https://i.imgur.com/ItziiKg.png) Актуальный [список поддерживаемых игр](https://github.com/rehlds/metamod-r/wiki/Поддерживаемые-игры)

## Build instructions
### Checking requirements

#### Windows
```
Visual Studio 2015 (C++14 standard) and later
```

#### Linux
```
git >= 1.8.5
cmake >= 3.10
GCC >= 4.9.2 (Optional)
ICC >= 15.0.1 20141023 (Optional)
LLVM (Clang) >= 6.0 (Optional)
```

### Building

#### Windows
Use `Visual Studio` to build, open `msvc/metamod.sln` and just select from the solution configurations list `Release` or `Debug`

#### Linux

* Optional options using `build.sh --compiler=[gcc] --jobs=[N] -D[option]=[ON or OFF]` (without square brackets)

```
-c=|--compiler=[icc|gcc|clang]  - Select preferred C/C++ compiler to build
-j=|--jobs=[N]                  - Specifies the number of jobs (commands) to run simultaneously (For faster building)
```

```
Definitions (-D):
DEBUG                           - Enables debugging mode
USE_STATIC_LIBSTDC              - Enables static linking library libstdc++
```

| Compiler           | Build command                    |
|--------------------|----------------------------------|
| ICC                |   `./build.sh --compiler=intel`  |
| LLVM (Clang)       |   `./build.sh --compiler=clang`  |
| GCC                |   `./build.sh --compiler=gcc`    |

##### Checking build environment (Debian / Ubuntu)

Installing required packages
```
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install -y gcc-multilib g++-multilib
sudo apt-get install -y build-essential
sudo apt-get install -y libc6-dev libc6-dev-i386
```

Select the preferred C/C++ Compiler installation
1) `sudo apt-get install -y gcc g++`
2) `sudo apt-get install -y clang`
