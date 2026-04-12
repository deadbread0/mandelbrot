mkdir -p obj
g++ simple_version.cpp -o obj/simple_version.o
g++ -I/SFML-3.0.2/include simple_version.cpp -lsfml-graphics -lsfml-system -lsfml-window -o simple_version.exe -O2 -march=native -DNDEBUG
./simple_version.exe 