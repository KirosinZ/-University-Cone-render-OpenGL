//
// Created by Kiril on 22.03.2022.
//

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(v3 pos, v3 up, float yaw, float pitch)
: Front(glm::vec3(0.0f, 1.0f, 0.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = pos;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up    = glm::normalize(glm::cross(Right, Front));
}

Camera::m4 Camera::view() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    if (direction == NONE) return;

    float velocity = MovementSpeed * deltaTime;
    v3 dir = v3(0.0f);
    if ((direction & FORWARD) == FORWARD)
    {
        glm::vec3 tmp(Front.x, 0.0f, Front.z);
        tmp = glm::normalize(tmp);
        dir += tmp;
        dir.y = 0.0f;
    }
    if ((direction & BACKWARD) == BACKWARD)
    {
        glm::vec3 tmp(Front.x, 0.0f, Front.z);
        tmp = glm::normalize(tmp);
        dir -= tmp;
        dir.y = 0.0f;
    }
    if ((direction & LEFT) == LEFT)
        dir -= Right;
    if ((direction & RIGHT) == RIGHT)
        dir += Right;
    if ((direction & UP) == UP)
        dir += WorldUp;
    if ((direction & DOWN) == DOWN)
        dir -= WorldUp;

    Position += dir * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 150.0f)
        Zoom = 150.0f;
}

