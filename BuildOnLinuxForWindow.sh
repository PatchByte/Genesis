rm -rf build_mingw

mkdir build_mingw
cd build_mingw

cmake -DCMAKE_TOOLCHAIN_FILE=../.cmake/LinuxSpooftoolchain.cmake ..
cmake --build . -j$(nproc)