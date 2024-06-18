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

## Installation

* Download latest genesis [release](https://github.com/yamp-project/genesis/releases/latest)
* Unzip it into an empty folder

## Using

* Run GenesisEditor.exe
* Press <kbd>File</kbd> -> <kbd>Open</kbd> and find `yamp.genesis` file that you want to edit
* Save yamp.genesis file after your job with <kbd>File</kbd> -> <kbd>Save</kbd>
* Compile `Offsets.hpp` file with <kbd>File</kbd> -> <kbd>Process specific file and save as</kbd> and select latest `GTA5_xxxx_dump.exe`, then select folder to save `Offsets.hpp`
* Now as temp solution commit your changes (`yamp.genesis` and `Offsets.hpp`) to genesis-output folder.<br>
**Tip: You can edit (`yamp.genesis` and `Offsets.hpp`) already in v-client submodule and after commit use them immediately**

## How to make and edit flows
In progress
