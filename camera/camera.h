//
// Created by Kiril on 22.03.2022.
//

#ifndef KG3_CAMERA_H
#define KG3_CAMERA_H

#include <glm/glm.hpp>
#include <GL/glew.h>

enum Camera_Movement {
    NONE = 0,
    FORWARD = 1,
    BACKWARD = 2,
    LEFT = 4,
    RIGHT = 8,
    UP = 16,
    DOWN = 32,
};

template<class Camera_Movement> inline Camera_Movement operator~ (Camera_Movement a) { return (Camera_Movement)~(int)a; }
template<class Camera_Movement> inline Camera_Movement operator| (Camera_Movement a, Camera_Movement b) { return (Camera_Movement)((int)a | (int)b); }
template<class Camera_Movement> inline Camera_Movement operator& (Camera_Movement a, Camera_Movement b) { return (Camera_Movement)((int)a & (int)b); }
template<class Camera_Movement> inline Camera_Movement operator^ (Camera_Movement a, Camera_Movement b) { return (Camera_Movement)((int)a ^ (int)b); }
template<class Camera_Movement> inline Camera_Movement& operator|= (Camera_Movement& a, Camera_Movement b) { return (Camera_Movement&)((int&)a |= (int)b); }
template<class Camera_Movement> inline Camera_Movement& operator&= (Camera_Movement& a, Camera_Movement b) { return (Camera_Movement&)((int&)a &= (int)b); }
template<class Camera_Movement> inline Camera_Movement& operator^= (Camera_Movement& a, Camera_Movement b) { return (Camera_Movement&)((int&)a ^= (int)b); }

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;

class Camera
{
    using v3 = glm::vec3;
    using m4 = glm::mat4;

    void updateCameraVectors();

public:
    v3 Position;
    v3 Front;
    v3 Up;
    v3 Right;
    v3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    explicit Camera(v3 pos, v3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);

    [[nodiscard]]
    m4 view() const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yoffset);
};


#endif //KG3_CAMERA_H
