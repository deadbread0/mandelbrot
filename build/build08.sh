mkdir -p obj
g++ vers08.cpp -o obj/vers08.o
g++ -I/SFML-3.0.2/include vers08.cpp -lsfml-graphics -lsfml-system -lsfml-window -o vers08.exe -O2 -march=native -DNDEBUG
./vers08.exe 