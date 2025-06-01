gcc main.c src/*.c --debug -Iinclude -Isrc -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o basicvm
./basicvm