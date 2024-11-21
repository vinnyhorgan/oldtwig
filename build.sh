gcc -DZ7_ST src/*.c src/api/*.c src/lib/minilua/*.c src/lib/stb/*.c src/lib/lzma/*.c -std=c99 -O2 -s -Isrc/lib/minilua -Isrc/lib/stb -Isrc/lib/lzma -lm $(pkg-config --cflags --libs sdl2) -o twig
