clang main.cpp src/**/*.cpp \
    -Iinclude -Isrc -I/Users/kimscicluna/raylib/src \
    -L/Users/kimscicluna/raylib/src -lraylib \
    -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL \
    -o main_macos -std=c++11

./main_macos