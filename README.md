# Genesis

Genesis was meant to be an easy Node-Based Pattern Editor to quickly scan for patterns after a game update of the specified target.
It is targetting Windows x86-64 PE Files.
It was originally used in YAMP.

## GenesisEditor

This is a gui editor for editing a genesis bundle file.

## GenesisRunner (TBD)

This is the command line tool which just runs the whole genesis bundle file and outputs the header file if no errors have occurred.

## Building

The build setup is very easy.

### Requirements
- Vulkan SDK / Libraries
  - Windows
    - https://www.lunarg.com/vulkan-sdk/
    - CLion | CLang | GCC
      - or anything other that builds with something else than MSVC because it has a compiler bug, we are not sure what it is causing it
  - Linux
    - Update the necessary dev, header and library packages
    - On fedora `sudo dnf install vulkan-loader vulkan-headers vulkan-loader-devel`
- Git SubModules
  - You can initialize them after clone with `git submodule update --init --recursive`

### Option 1: (Any editor with CMake Support)

Just open this project in the code editor and build it with cmake.

### Option 2: (CMD)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Installation

* Download latest genesis [release](https://github.com/yamp-project/genesis/releases/latest)
* Unzip it into an empty folder
