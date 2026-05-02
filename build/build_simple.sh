mkdir -p obj
g++ src/simple_version.cpp -o obj/simple_version.o
g++ -I/SFML-3.0.2/include src/simple_version.cpp -lsfml-graphics -lsfml-system -lsfml-window -o src/simple_version.exe -O3 -march=native -DNDEBUG
./simple_version.exe 