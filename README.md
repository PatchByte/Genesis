# Genesis

Genesis is meant to solve the problem of finding outdated and updating functions, class members, pointers etc.

## GenesisEditor

This is a gui editor for editing a genesis bundle file.

## GenesisRunner (TBA)

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

### Option 1: (VSCode)

Just open this project in VSCode and build with the cmake support.

### Option 2: (CMD)

```bash
mkdir build
cd build
cmake ..
cmake --build .
```