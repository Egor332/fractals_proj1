rm -f *gcda
gcc \
src/*.c \
-O3 \
-flto \
-ffast-math \
-fwhole-program \
-fomit-frame-pointer \
-fstrict-aliasing \
-fprofile-generate \
-march=native \
-mtune=native \
-Wall \
-Wstrict-aliasing \
-lglfw -lpthread -I src/glad/include

#-funroll-loops \
