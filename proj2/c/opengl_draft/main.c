#include <glad.h> 
#include <GLFW/glfw3.h> 
#include <stdlib.h>     
#include <time.h>       
#include "update_callback.h"

const char* const VERTEX_SHADER_SRC =
"    #version 330 core                                               \n"
"    layout (location = 0) in vec2 aPos;                             \n"
"    layout (location = 1) in vec3 aColor;                           \n"
"    out vec3 outColor;                                              \n"
"                                                                      "
"    void main()                                                       "
"    {                                                                 "
"        float width = (" PERC_STRING(WORLD_WIDTH) ");                 "
"        float height = (" PERC_STRING(WORLD_HEIGHT) ");               "
"        float cell_size = " PERC_STRING(CELL_SIZE) ";                 "
"        float tx = aPos.x * 2.0 / cell_size / width - 1.0;            "
"        float ty = 1.0 - aPos.y * 2.0 / cell_size / height;           "
"        gl_Position = vec4(tx, ty, 0.0, 1.0);                         "
"        outColor = aColor;                                            "
"    }                                                                 ";

const char* const FRAGMENT_SHADER_SRC =
"    #version 330 core                                               \n"
"    in vec3 outColor;                                               \n"
"    out vec4 FragColor;                                             \n"
"                                                                      "
"    void main()                                                       "
"    {                                                                 "
"        //float gamma = 2.2;                                        \n"
"        //vec3 gammaColor = pow(outColor, vec3(1.0/gamma));         \n"
"                                                                      "
"        FragColor = vec4(outColor, 1.0);                              "
"    }                                                                 ";

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void process_input(GLFWwindow* window, int* stop)
{
    if (glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS)
        *stop = 1;
}

int simulate()
{
    const float FLT_OUTLINE_THICKNESS = 0;

    struct Vertex* vertices;
    unsigned int* indices;
    GLFWwindow* window;
    int vertex_shader;
    int fragment_shader;
    int shader_program;
    int success;
    struct Vertex* outline_vertices;
    unsigned int* outline_indices;
    unsigned int VBO, VAO, EBO;
    
    vertices = (struct Vertex*)malloc(WORLD_WIDTH * WORLD_HEIGHT * 12 * sizeof(struct Vertex));
    if (!vertices)
    {
        return 1;
    }

    indices = (unsigned int*)malloc(WORLD_WIDTH * WORLD_HEIGHT * 30 * sizeof(unsigned int));
    if (!indices)
    {
        free(vertices);
        return 1;
    }

    outline_vertices = vertices + WORLD_WIDTH * WORLD_HEIGHT * 4;
    outline_indices = indices + WORLD_WIDTH * WORLD_HEIGHT * 6;

    if (!glfwInit())
    {
        free(vertices);
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSwapInterval(1);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(WORLD_WIDTH * CELL_SIZE, WORLD_HEIGHT * CELL_SIZE, "Nice Simulation", glfwGetPrimaryMonitor(), NULL);
    if (!window)
    {
        glfwTerminate();
        free(vertices);
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        free(vertices);
        return 1;
    }

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &VERTEX_SHADER_SRC, NULL);
    glCompileShader(vertex_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertex_shader);
        glfwTerminate();
        free(vertices);
        return 1;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &FRAGMENT_SHADER_SRC, NULL);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glfwTerminate();
        free(vertices);
        return 1;
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        glDeleteProgram(shader_program);
        glfwTerminate();
        free(vertices);
        return 1;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    for (int i = 0; i < WORLD_WIDTH * WORLD_HEIGHT; ++i)
    {
        unsigned int* inner_indices = indices + i * 6;
        unsigned int* outer_indices = outline_indices + i * 24;

        inner_indices[0] = i * 4 + 0;
        inner_indices[1] = i * 4 + 1;
        inner_indices[2] = i * 4 + 2;
        inner_indices[3] = i * 4 + 2;
        inner_indices[4] = i * 4 + 3;
        inner_indices[5] = i * 4 + 0;

        for (int j = 0; j < 24; ++j)
            outer_indices[j] = WORLD_WIDTH * WORLD_HEIGHT * 4 + i * 8;

        outer_indices[0] += 0;
        outer_indices[1] += 1;
        outer_indices[2] += 3;
        outer_indices[3] += 3;
        outer_indices[4] += 1;
        outer_indices[5] += 2;

        outer_indices[6] += 3;
        outer_indices[7] += 2;
        outer_indices[8] += 7;
        outer_indices[9] += 7;
        outer_indices[10] += 2;
        outer_indices[11] += 6;

        outer_indices[12] += 7;
        outer_indices[13] += 6;
        outer_indices[14] += 5;
        outer_indices[15] += 5;
        outer_indices[16] += 4;
        outer_indices[17] += 7;

        outer_indices[18] += 0;
        outer_indices[19] += 1;
        outer_indices[20] += 4;
        outer_indices[21] += 4;
        outer_indices[22] += 5;
        outer_indices[23] += 1;
    }

    const float FLT_CELL_SIZE = CELL_SIZE;

    for (int y = 0; y < WORLD_HEIGHT; ++y)
    {
        for (int x = 0; x < WORLD_WIDTH; ++x)
        {
            struct Vertex* inner_vertices = vertices + (y * WORLD_WIDTH + x) * 4;
            struct Vertex* outer_vertices = outline_vertices + (y * WORLD_WIDTH + x) * 8;

            inner_vertices[0].x = x * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            inner_vertices[0].y = y * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            inner_vertices[1].x = (x + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            inner_vertices[1].y = y * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            inner_vertices[2].x = (x + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            inner_vertices[2].y = (y + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            inner_vertices[3].x = x * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            inner_vertices[3].y = (y + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;

            outer_vertices[0].x = x * FLT_CELL_SIZE;
            outer_vertices[0].y = y * FLT_CELL_SIZE;
            outer_vertices[1].x = x * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            outer_vertices[1].y = y * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            outer_vertices[2].x = (x + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            outer_vertices[2].y = y * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            outer_vertices[3].x = (x + 1) * FLT_CELL_SIZE;
            outer_vertices[3].y = y * FLT_CELL_SIZE;
            outer_vertices[4].x = x * FLT_CELL_SIZE;
            outer_vertices[4].y = (y + 1) * FLT_CELL_SIZE;
            outer_vertices[5].x = x * FLT_CELL_SIZE + FLT_OUTLINE_THICKNESS;
            outer_vertices[5].y = (y + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            outer_vertices[6].x = (x + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            outer_vertices[6].y = (y + 1) * FLT_CELL_SIZE - FLT_OUTLINE_THICKNESS;
            outer_vertices[7].x = (x + 1) * FLT_CELL_SIZE;
            outer_vertices[7].y = (y + 1) * FLT_CELL_SIZE;
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, WORLD_WIDTH * WORLD_HEIGHT * 12 * sizeof(struct Vertex), NULL, GL_STREAM_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, WORLD_WIDTH * WORLD_HEIGHT * 30 * sizeof(unsigned int), (void*)indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, x));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(struct Vertex), (void*)offsetof(struct Vertex, r));
    glEnableVertexAttribArray(1);

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glUseProgram(shader_program);

    unsigned long long capture_counter = 0;

    typedef unsigned char uns_char;
    uns_char* pixels = malloc(3 * WORLD_WIDTH * CELL_SIZE * WORLD_HEIGHT * CELL_SIZE);

    struct Context cx;
    
    if (!init_callback(vertices, WORLD_WIDTH, WORLD_HEIGHT, 4, &cx)) {
        goto FAILED;
    }

    while (!glfwWindowShouldClose(window))
    {
        int stop = 0;
        process_input(window, &stop);

        if (!update_callback(vertices, WORLD_WIDTH, WORLD_HEIGHT, 4, &cx)) {
            goto FAILED;
        }

        glBufferSubData(GL_ARRAY_BUFFER, 0, WORLD_WIDTH * WORLD_HEIGHT * 12 * sizeof(struct Vertex), (void*)vertices);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, WORLD_WIDTH * WORLD_HEIGHT * 30, GL_UNSIGNED_INT, NULL);

        glfwSwapBuffers(window);
        glfwPollEvents();

        ++capture_counter;
        if (capture_counter == PERC_MAX_LVL * PERC_FINISH_RATIO_NUM / PERC_FINISH_RATIO_DEN) {
            break;
        }

        if (stop) break;
    }

    finish_callback(vertices, WORLD_WIDTH, WORLD_HEIGHT, 4, &cx);

FAILED:

    glfwDestroyWindow(window);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shader_program);
    glfwTerminate();
    
    free(pixels);
    free(vertices);
    free(indices);
    return 0;
}

int main() {
    return simulate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


