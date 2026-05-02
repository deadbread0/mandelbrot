mkdir -p obj
g++ src/vers_func.cpp -o obj/vers_func.o
g++ -I/SFML-3.0.2/include src/vers_func.cpp -lsfml-graphics -lsfml-system -lsfml-window -o src/vers_func.exe -O3 -march=native -DNDEBUG
./vers_func.exe 