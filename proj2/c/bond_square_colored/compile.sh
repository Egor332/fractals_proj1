gcc \
*.c ../lib/*.c \
-O3 \
-flto \
-ffast-math \
-fwhole-program \
-fomit-frame-pointer \
-fstrict-aliasing \
-march=native \
-mtune=native \
-Wall \
-Wstrict-aliasing \
-std=c99 \
-o main \
-lglfw -lpthread -I ../lib/
