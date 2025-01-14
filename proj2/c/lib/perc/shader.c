// http://learnopengl.com/
#include "shader.h"
#include <stdio.h>
#include <stddef.h>

void shd_init(GLuint* shd, const char *const vertex,
              const char *const fragment,
              const char *const geometry) {
    GLuint vx, fr, ge;
    vx = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vx, 1, &vertex, NULL);
    glCompileShader(vx);
    shd_checkCompileErrors(vx, 0);

    fr = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fr, 1, &fragment, NULL);
    glCompileShader(fr);
    shd_checkCompileErrors(fr, 1);

    if (geometry) {
        ge = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(ge, 1, &geometry, NULL);
        glCompileShader(ge);
        shd_checkCompileErrors(ge, 2);
    }

    *shd = glCreateProgram();
    glAttachShader(*shd, vx);
    glAttachShader(*shd, fr);
    if (geometry) {
        glAttachShader(*shd, ge);
    }

    glLinkProgram(*shd);
    shd_checkCompileErrors(*shd, 3);

    glDeleteShader(vx);
    glDeleteShader(fr);
    if (geometry) {
        glDeleteShader(ge);
    }
}

void shd_checkCompileErrors(GLuint shd, int err) {
    GLint success;
    GLchar infoLog[1024];
    if (err != 3) {
        glGetShaderiv(shd, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shd, 1024, NULL, infoLog);
            fprintf(stderr, "[compile] %1024s\n", infoLog);
        }
    } else {
        glGetProgramiv(shd, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shd, 1024, NULL, infoLog);
            fprintf(stderr, "[link] %1024s\n", infoLog);
        }

    }
}
