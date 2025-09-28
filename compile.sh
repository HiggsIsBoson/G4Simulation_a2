rm -rf build; mkdir build; cd build
cmake -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DZLIB_ROOT="$CONDA_PREFIX" ../
make -j$(sysctl -n hw.ncpu)
cd ..
