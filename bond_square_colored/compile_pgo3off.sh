rm -f *gcda
gcc \
src/*.c \
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
-o perc \
-lglfw -lpthread -I src/glad/include
