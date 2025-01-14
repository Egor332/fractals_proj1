/* http://learnopengl.com/ */
#ifndef SHADER_H
#define SHADER_H
#include <glad.h>

void shd_init(GLuint* shd, const char * const vertex,
              const char * const fragment,
              const char * const geometry);

void shd_checkCompileErrors(GLuint shd, int err);

#endif
