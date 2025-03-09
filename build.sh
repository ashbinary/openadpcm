echo "Building converter..."
g++ -O2 -std=c++0x src/converter.cpp -o brstm_converter -Wall
#x86_64-w64-mingw32-g++ -static-libgcc -static-libstdc++ -o bwav_converter.exe src/converter.cpp