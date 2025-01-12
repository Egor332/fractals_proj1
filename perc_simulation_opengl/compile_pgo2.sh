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
-lglfw -lpthread -I src/glad/include

#-funroll-loops \
