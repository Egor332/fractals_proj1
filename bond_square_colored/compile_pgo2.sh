gcc \
src/*.c \
-O3 \
-flto \
-ffast-math \
-fwhole-program \
-fomit-frame-pointer \
-fstrict-aliasing \
-fprofile-use \
-march=native \
-mtune=native \
-Wall \
-Wstrict-aliasing \
-std=c99 \
-o perc \
-lglfw -lpthread -I src/glad/include
rm -f *gcda
