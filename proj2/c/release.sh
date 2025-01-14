gcc \
*.c \
../lib/perc/*.c \
../lib/perc3d/*.c \
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
-pipe \
-std=c99 \
-o main \
-lglfw -lpthread -lm \
-I $HOME/cglm-0.9.4/include/ \
-I ../lib/perc/ \
-I ../lib/perc3d/
