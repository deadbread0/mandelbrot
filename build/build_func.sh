mkdir -p obj
g++ vers_func.cpp -o obj/vers_func.o
g++ -I/SFML-3.0.2/include vers_func.cpp -lsfml-graphics -lsfml-system -lsfml-window -o vers_func.exe -O2 -march=native -DNDEBUG
./vers_func.exe 