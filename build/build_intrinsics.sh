mkdir -p obj
g++ intrinsics_version.cpp -o obj/intrinsics_version.o
g++ -I/SFML-3.0.2/include intrinsics_version.cpp -lsfml-graphics -lsfml-system -lsfml-window -o intrinsics_version.exe -O2 -march=native -DNDEBUG -mavx -mavx2
./intrinsics_version.exe 