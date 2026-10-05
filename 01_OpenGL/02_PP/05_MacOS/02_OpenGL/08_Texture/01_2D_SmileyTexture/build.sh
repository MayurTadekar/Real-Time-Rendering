
clear

mkdir -p smiley.app/Contents/MacOS

clang -Wno-deprecated-declarations -o smiley.app/Contents/MacOS/smiley OGL.mm -framework Cocoa -framework QuartzCore -framework OpenGL
