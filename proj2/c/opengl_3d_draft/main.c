/* https://learnopengl.com */
#include <global_config.h>
#include <shader.h>
#include <camera.h>
#include <GLFW/glfw3.h>

#define SHADER_VERT \
"#version 330 core                          \n" \
"layout (location = 0) in vec3 aPos;        \n" \
"layout (location = 1) in vec3 aNormal;     \n" \
"                                             " \
"out vec3 FragPosView;                        " \
"out vec3 Normal;                             " \
"                                             " \
"uniform mat4 model;                          " \
"uniform mat4 view;                           " \
"uniform mat4 projection;                     " \
"uniform mat3 normViewModelMatrix;            " \
"                                             " \
"void main()                                  " \
"{                                            " \
"    vec4 worldPos = model * vec4(aPos, 1.0); " \
"    vec4 viewPos = view * worldPos;          " \
"    FragPosView = vec3(viewPos);             " \
"    Normal = normViewModelMatrix * aNormal;  " \
"    gl_Position = projection * viewPos;      " \
"}                                            "

#define SHADER_FRAG \
"#version 330 core                                                    \n" \
"#define light_ambientLightColor vec3(0.1, 0.1, 0.1)                  \n" \
"#define light_diffuseLightColor vec3(0.5, 0.5, 0.5)                  \n" \
"#define light_specularLightColor vec3(0.5, 0.5, 0.5)                 \n" \
"                                                                       " \
"uniform vec3 light_lightPosView;                                       " \
"                                                                       " \
"out vec4 FragColor;                                                    " \
"                                                                       " \
"in vec3 Normal;                                                        " \
"in vec3 FragPosView;                                                   " \
"                                                                       " \
"float get_attenuation(float dist) {                                    " \
"                                                                       " \
"    float light_constant = 1.0;                                        " \
"    float light_linear = 0.0001;                                       " \
"                                                                       " \
"    return 1.0 / (light_constant + light_linear * dist);               " \
"}                                                                      " \
"                                                                       " \
"vec3 calc_diffuse_point_light(vec3 normal,                             " \
"vec3 fragPosView, vec3 viewDir) {                                      " \
"    vec3 lightDir   = normalize(light_lightPosView - fragPosView);     " \
"    float diff      = max(dot(normal, lightDir), 0.0);                 " \
"    float dist      = length(light_lightPosView - fragPosView);        " \
"    vec3 diffuse    = light_diffuseLightColor * diff;                  " \
"    diffuse         = diffuse * get_attenuation(dist);                 " \
"    return            diffuse;                                         " \
"}                                                                      " \
"                                                                       " \
"vec3 calc_specular_point_light(vec3 normal,                            " \
"    vec3 fragPosView, vec3 viewDir) {                                  " \
"    float shininess = 64;                                              " \
"                                                                       " \
"    vec3 lightDir   = normalize(light_lightPosView - fragPosView);     " \
"                                                                       " \
"    vec3 reflectDir = reflect(-lightDir, normal);                      " \
"    vec3 halfwayDir = normalize(lightDir + viewDir);                   " \
"    //float spec      = pow(max(dot(normal, halfwayDir), 0.0),         " \
"                    shininess);                                      \n" \
"    float spec      = pow(max(dot(viewDir, reflectDir), 0.0),          " \
"      shininess/2.0);                                                \n" \
"                                                                       " \
"    float dist      = length(light_lightPosView - fragPosView);        " \
"    vec3 specular   = light_specularLightColor * spec;                 " \
"    specular        = specular * get_attenuation(dist);                " \
"    return            specular;                                        " \
"}                                                                      " \
"                                                                       " \
"void main() {                                                          " \
"                                                                       " \
"    vec3 norm = normalize(Normal);                                     " \
"    vec3 viewDir = normalize(-FragPosView);                            " \
"    vec4 Position = vec4(FragPosView, 1.0);                            " \
"                                                                       " \
"    vec3 ambient = vec3(0.0);                                          " \
"                                                                       " \
"    ambient = ambient + light_ambientLightColor;                       " \
"                                                                       " \
"    vec3 diffuse_result = vec3(0.0);                                   " \
"    diffuse_result += calc_diffuse_point_light(norm,                   " \
"vec3(Position), viewDir);                                              " \
"    vec3 diffuse = diffuse_result;                                     " \
"                                                                       " \
"    vec3 specular_result = vec3(0.0);                                  " \
"    specular_result += calc_specular_point_light(norm,                 " \
"vec3(Position), viewDir);                                              " \
"    vec3 specular = specular_result;                                   " \
"                                                                       " \
"    vec3 result = ambient + diffuse + specular;                        " \
"                                                                       " \
"    float gamma = 2.2;                                                 " \
"                                                                       " \
"    result = pow(result, vec3(1.0 / gamma));                           " \
"    FragColor = vec4(result, 1.0);                                     " \
"                                                                       " \
"                                                                       " \
"}                                                                      "

typedef struct GlobalAttributes_ {
    
    Camera camera;

    int firstMouse;

    float deltaTime;
    float lastFrame;

    float lastX;
    float lastY;

} GlobalAttributes;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void normMatrix(mat4 mx, mat3 dest) {
    mat4 tmp;
    glm_mat4_inv_fast(mx, tmp);
    glm_mat4_transpose(tmp);
    glm_mat4_pick3(tmp, dest);
}

GlobalAttributes* callback_attributes = NULL;

int main()
{
    GlobalAttributes attr;
    callback_attributes = &attr;

    cam_init(&attr.camera);
    attr.firstMouse = 1;
    attr.deltaTime = 0;
    attr.lastFrame = 0;
    attr.lastX = SCR_WIDTH / 2.f;
    attr.lastY = SCR_HEIGHT / 2.f;

    glfwInit();
    glfwWindowHint(GLFW_SAMPLES, 4); // antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
        "Nice 3D simulation", glfwGetPrimaryMonitor(), NULL); // fullscreen

    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }


    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    float vertices[] = {
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        1.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        1.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        0.0f, 1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
        0.0f, 0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

        0.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        1.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        1.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        0.0f, 1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
        0.0f, 0.0f, 1.0f,  0.0f,  0.0f,  1.0f,

        0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        0.0f, 1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
        0.0f, 0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
        0.0f, 1.0f, 1.0f, -1.0f,  0.0f,  0.0f,

        1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, 1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, 1.0f, 1.0f,  1.0f,  0.0f,  0.0f,

        0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        1.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        0.0f, 0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
        0.0f, 0.0f, 0.0f,  0.0f, -1.0f,  0.0f,

        0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        1.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
        0.0f, 1.0f, 0.0f,  0.0f,  1.0f,  0.0f
    };
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
            GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
            (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
            (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    GLuint shd;
    shd_init(&shd, SHADER_VERT, SHADER_FRAG, NULL);

    glUseProgram(shd);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        attr.deltaTime = currentFrame - attr.lastFrame;
        attr.lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.2f, 0.2f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 projection;
        glm_perspective(glm_rad(attr.camera.Zoom),
                (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 10000.f, projection);

        mat4 sunModel;
        vec3 tmp1;
        tmp1[0] = -2500.f; tmp1[1] = 2500.f; tmp1[2] = -4000.f;
        vec3 tmp2;
        tmp2[0] = 30; tmp2[1] = 30; tmp2[2] = 30;
        glm_mat4_identity(sunModel);
        glm_translate(sunModel, tmp1);
        glm_scale(sunModel, tmp2);

        mat4 model;
        glm_mat4_identity(model);

        mat4 view;
        cam_view(&attr.camera, view);
       
        mat4 norm_tmp;
        mat3 norm;
        glm_mat4_copy(model, norm_tmp);
        glm_mat4_mul(view, norm_tmp, norm_tmp);
        normMatrix(norm_tmp, norm);

        vec3 lightPos;
        vec4 tmp3;
        tmp3[0] = 0; tmp3[1] = 0; tmp3[2] = 0; tmp3[3] = 1;
        glm_mat4_mulv(sunModel, tmp3, tmp3);
        glm_mat4_mulv(view, tmp3, tmp3);
        glm_vec4_copy3(tmp3, lightPos);

        glUniformMatrix4fv(glGetUniformLocation(shd, "view"), 1, GL_FALSE,
                (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(shd, "projection"), 1,
                GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(shd, "light_lightPosView"), 1,
                lightPos);
        glUniformMatrix4fv(glGetUniformLocation(shd, "model"), 1,
                GL_FALSE, (float*)model);
        glUniformMatrix3fv(glGetUniformLocation(shd,
                    "normViewModelMatrix"), 1, GL_FALSE, (float*)norm);
        
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

void processInput(GLFWwindow *window)
{
    if (!callback_attributes)
        return;

    GlobalAttributes* attr = callback_attributes;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam_key(&attr->camera, CAM_FORWARD, attr->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam_key(&attr->camera, CAM_BACKWARD, attr->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam_key(&attr->camera, CAM_LEFT, attr->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam_key(&attr->camera, CAM_RIGHT, attr->deltaTime);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!callback_attributes)
        return;

    GlobalAttributes* attr = callback_attributes;

    if (attr->firstMouse)
    {
        attr->lastX = xpos;
        attr->lastY = ypos;
        attr->firstMouse = 0;
    }

    float xoffset = xpos - attr->lastX;
    float yoffset = attr->lastY - ypos;

    attr->lastX = xpos;
    attr->lastY = ypos;

    cam_mov(&attr->camera, xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (!callback_attributes)
        return;

    GlobalAttributes* attr = callback_attributes;

    cam_scr(&attr->camera, yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

