clear

g++ OGL.cpp -o OGL.out -lGL -lGLU -lX11 -lGLEW -L. -lSphere -Wl,-rpath,'$ORIGIN'

./OGL.out
