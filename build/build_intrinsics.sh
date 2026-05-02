mkdir -p obj
g++ src/intrinsics_version.cpp -o obj/intrinsics_version.o
g++ -I/SFML-3.0.2/include src/intrinsics_version.cpp -lsfml-graphics -lsfml-system -lsfml-window -o src/intrinsics_version.exe -O3 -march=native -DNDEBUG -mavx -mavx2
./intrinsics_version.exe 