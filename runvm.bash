gcc main.c src/*.c -Iinclude -Isrc -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o basicvm
./basicvm