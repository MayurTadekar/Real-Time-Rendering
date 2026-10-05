clear

# delete all the object files and the output executable
rm -f *.o OGL.out

gcc -c -o OGL.o OGL.c 
gcc -c -o ./alphabets.o ./alphabets/alphabets.c 
gcc -c -o ./alphabet_A.o ./alphabets/alphabet_A.c 
gcc -c -o ./alphabet_B.o ./alphabets/alphabet_B.c 
gcc -c -o ./alphabet_H.o ./alphabets/alphabet_H.c
gcc -c -o ./alphabet_R.o ./alphabets/alphabet_R.c
gcc -c -o ./alphabet_T.o ./alphabets/alphabet_T.c
gcc -c -o pathtracer.o pathtracer.c 
gcc -c -o ashoka_chakra.o ashoka_chakra.c
gcc -c -o lerpbox.o lerpbox.c
gcc -c -o dynamic_BHARAT_animation.o dynamic_BHARAT_animation.c
gcc -c -o fighter_jet.o fighter_jet.c

gcc -o OGL.out OGL.o alphabets.o alphabet_A.o alphabet_B.o alphabet_H.o alphabet_R.o alphabet_T.o pathtracer.o lerpbox.o dynamic_BHARAT_animation.o fighter_jet.o ashoka_chakra.o -lX11 -lc -lGL -lGLU -lm -lopenal -lmpg123

./OGL.out

