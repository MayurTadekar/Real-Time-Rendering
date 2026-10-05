clear

gcc -c -o OGL.o OGL.c

gcc -o OGL.out OGL.o -lX11 -lc -lGL -lGLU

./OGL.out

