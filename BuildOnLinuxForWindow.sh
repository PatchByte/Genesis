rm -rf build_mingw

mkdir build_mingw
cd build_mingw

cmake -DCMAKE_TOOLCHAIN_FILE=../.cmake/LinuxSpooftoolchain.cmake \
      -DOPENSSL_ROOT_DIR=/usr/x86_64-w64-mingw32/sys-root/mingw \
      -DOPENSSL_CRYPTO_LIBRARY=/usr/x86_64-w64-mingw32/sys-root/mingw/lib/libssl.a \
      -DNO_EXAMPLES=ON \
      -DOPENSSL_CRYPTO_INCLUDE_DIR=/usr/x86_64-w64-mingw32/sys-root/mingw/include/openssl ..

cmake --build . -j$(nproc)