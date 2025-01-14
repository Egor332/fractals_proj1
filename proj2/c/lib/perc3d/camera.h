/* http://learnopengl.com/ */
#ifndef CAMERA_H
#define CAMERA_H
#include <cglm/cglm.h>

typedef enum Camera_Movement_
{
    CAM_FORWARD,
    CAM_BACKWARD,
    CAM_LEFT,
    CAM_RIGHT
} Camera_Movement;

#define CAM_YAW  (-90.0f)
#define CAM_PITCH  0.0f
#define CAM_SPEED  10.f
#define CAM_SENSITIVITY  0.1f
#define CAM_ZOOM  45.0f

typedef struct Camera_
{
    vec3 Position;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;
} Camera;

void cam_view(Camera* cam, mat4 dest);
void cam_init(Camera* cam);
void cam_key(Camera* cam, Camera_Movement dir, float deltaTime);
void cam_mov(Camera* cam, float xoffset, float yoffset);
void cam_scr(Camera* cam, float yoffset);
void cam_upd(Camera* cam);

#endif 
