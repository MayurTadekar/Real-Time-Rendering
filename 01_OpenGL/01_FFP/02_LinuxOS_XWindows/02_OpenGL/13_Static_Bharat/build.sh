clear

gcc -c -o OGL.o OGL.c 
gcc -c -o ./alphabets.o ./alphabets/alphabets.c 
gcc -c -o ./alphabet_A.o ./alphabets/alphabet_A.c 
gcc -c -o ./alphabet_B.o ./alphabets/alphabet_B.c 
gcc -c -o ./alphabet_H.o ./alphabets/alphabet_H.c
gcc -c -o ./alphabet_R.o ./alphabets/alphabet_R.c
gcc -c -o ./alphabet_T.o ./alphabets/alphabet_T.c
gcc -c -o pathtracer.o pathtracer.c 
gcc -c -o lerpbox.o lerpbox.c

gcc -o OGL.out OGL.o alphabets.o alphabet_A.o alphabet_B.o alphabet_H.o alphabet_R.o alphabet_T.o pathtracer.o lerpbox.o -lX11 -lc -lGL -lGLU -lm

./OGL.out

