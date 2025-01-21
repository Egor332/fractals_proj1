gcc \
*.c \
../lib/perc/*.c \
../lib/perc3d/*.c \
-g \
-Wall \
-fsanitize=address,undefined \
-march=native \
-mtune=native \
-Wstrict-aliasing \
-Werror=vla \
-pipe \
-std=c99 \
-o main \
-lglfw -lpthread -lm \
-I $HOME/cglm-0.9.4/include/ \
-I ../lib/perc/ \
-I ../lib/perc3d/
