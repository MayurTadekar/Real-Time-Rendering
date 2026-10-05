cls

gcc -c -o xwindow.o xwindow.c

gcc -o xwindow.out xwindow.o -lX11 -lc

./xwindow
