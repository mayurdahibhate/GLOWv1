rm -f OGL.o OGL

g++ -c -o OGL.o -I ../GLOW/include/ OGL.cpp
g++ -o OGL -L /usr/lib/x86_64-linux-gnu OGL.o window.o -lX11 -lGL -L ../GLOW/lib -lglad -lassimp
