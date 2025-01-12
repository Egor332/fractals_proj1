#include <glad/glad.h>  
#include <GLFW/glfw3.h> 
#include <stdlib.h>     
#include "update_callback.h"

int main() {
    uint32_t* pixels;
    GLFWwindow* window;

    pixels = malloc(sizeof(uint32_t) * WORLD_WIDTH * WORLD_HEIGHT);
    if (!pixels) {
        return 1;
    }

    if (!glfwInit()) {
        free(pixels);
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(1);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(WORLD_WIDTH, WORLD_HEIGHT, "Nice Simulation", glfwGetPrimaryMonitor(), NULL);
    if (!window) {
        goto FAILED;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        goto FAILED;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WORLD_WIDTH, WORLD_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    /* simulation context */
    struct Context cx;
    
    if (!init_callback(pixels,  &cx)) {
        goto FAILED;
    }

    int pause = 0;

    while (!glfwWindowShouldClose(window)) {
        if (!update_callback(pause, pixels, &cx)) {
            goto FAILED;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WORLD_WIDTH, WORLD_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glBlitFramebuffer(0, 0, WORLD_WIDTH, WORLD_HEIGHT, 0, 0, WORLD_WIDTH, WORLD_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (cx.level == PERC_MAX_LVL) {
            break;
        }
        if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
            break;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            pause = 1;
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            pause = 0;
        }
    }

    finish_callback(pixels, &cx);

FAILED:

    glfwDestroyWindow(window);
    glfwTerminate();
    free(pixels);
    return 0;
}
