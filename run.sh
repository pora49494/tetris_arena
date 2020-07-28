DIR="./.tetris_arena" 
if [ ! -d "$DIR" ]; then
    mkdir "$DIR"
fi

g++ -c Play.cpp -o "$DIR"/play.o 
g++ "$DIR"/play.o -o "$DIR"/app -lsfml-graphics -lsfml-window -lsfml-system -lpthread 
"$DIR"/app
 