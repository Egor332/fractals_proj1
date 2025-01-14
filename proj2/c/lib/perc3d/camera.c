/* http://learnopengl.com/ */
#include "camera.h"
#include <math.h>

void cam_init(Camera* cam) {
    glm_vec3_zero(cam->Position);
    glm_vec3_zero(cam->Front);
    glm_vec3_zero(cam->Up);
    glm_vec3_zero(cam->Right);
    glm_vec3_zero(cam->WorldUp);

    cam->Front[2] = -1.0f;
    cam->WorldUp[1] = 1.0f;
    
    cam->Yaw = CAM_YAW;
    cam->Pitch = CAM_PITCH;
    cam->MovementSpeed = CAM_SPEED;
    cam->MouseSensitivity = CAM_SENSITIVITY;
    cam->Zoom = CAM_ZOOM;

    cam_upd(cam);
}

void cam_upd(Camera* cam) {
    vec3 front;
    
    front[0] = cos(glm_rad(cam->Yaw))*cos(glm_rad(cam->Pitch));
    front[1] = sin(glm_rad(cam->Pitch));
    front[2] = sin(glm_rad(cam->Yaw))*cos(glm_rad(cam->Pitch));
    
    glm_normalize_to(front, cam->Front);
    
    glm_cross(cam->Front, cam->WorldUp, cam->Right);
    glm_normalize(cam->Right);
    
    glm_cross(cam->Right, cam->Front, cam->Up);
    glm_normalize(cam->Up);
}

void cam_view(Camera* cam, mat4 dest) {
    vec3 tmp;
    glm_vec3_add(cam->Position, cam->Front, tmp);
    glm_lookat(cam->Position, tmp, cam->Up, dest);
}

void cam_key(Camera* cam, Camera_Movement dir, float deltaTime) {
    float velocity = cam->MovementSpeed * deltaTime;
    vec3 tmp;
    if (dir == CAM_FORWARD) {
        glm_vec3_scale(cam->Front, velocity, tmp);
        glm_vec3_add(cam->Position, tmp, cam->Position);
    }
    if (dir == CAM_BACKWARD) {
        glm_vec3_scale(cam->Front, velocity, tmp);
        glm_vec3_sub(cam->Position, tmp, cam->Position);
    }
    if (dir == CAM_LEFT) {
        glm_vec3_scale(cam->Right, velocity, tmp);
        glm_vec3_sub(cam->Position, tmp, cam->Position);
    }
    if (dir == CAM_RIGHT) {
        glm_vec3_scale(cam->Right, velocity, tmp);
        glm_vec3_add(cam->Position, tmp, cam->Position);
    }
}

void cam_mov(Camera* cam, float xoffset, float yoffset) {
    xoffset *= cam->MouseSensitivity;
    yoffset *= cam->MouseSensitivity;

    cam->Yaw += xoffset;
    cam->Pitch += yoffset;

    if (cam->Pitch > 89.f) cam->Pitch = 89.f;
    if (cam->Pitch < -89.f) cam->Pitch = -89.f;

    cam_upd(cam);
}

void cam_scr(Camera* cam, float yoffset) {
    if (cam->Zoom >= 1 && cam->Zoom <= 45) {
        cam->Zoom -= yoffset;
    }
    if (cam->Zoom <= 1) {
        cam->Zoom = 1;
    }
    if (cam->Zoom >= 45) {
        cam->Zoom = 45;
    }
}
