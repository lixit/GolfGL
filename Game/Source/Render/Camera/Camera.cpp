#include <precomp.h>

#include "Camera.h"

void Camera::Init()
{    
    ms_Camera = std::move( std::unique_ptr<Camera>() );
}

void Camera::Look(Direction direction, float dt)
{

    float distance = MovementSpeed * dt;
    switch (direction)
    {
    case Direction::FORWARD:
        cameraPos += distance * cameraFront;
        break;
    case Direction::BACKWARD:
        cameraPos -= distance * cameraFront;
        break;
    case Direction::LEFT:
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * distance;
        break;
    case Direction::RIGHT:
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * distance;
        break;
    case Direction::UP:
        cameraPos += distance * cameraUp;
        break;
    case Direction::DOWN:
        cameraPos -= distance * cameraUp;
        break;
    default:
        break;
    }
}

glm::mat4 Camera::LookAt()
{
    return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}


const glm::vec3& Camera::GetCameraPos()
{
    return cameraPos;
}

float Camera::GetCameraZoom()
{
    return Zoom;
}

const glm::vec3& Camera::GetCameraFront()
{
    return cameraFront;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    m_BoundAngleInRange(Pitch, -constrainPitchBound, constrainPitchBound);    

    // update Front, Right and Up Vectors using the updated Euler angles
    m_UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= static_cast<float>(yoffset);
    m_BoundAngleInRange(Zoom, zoomLowerBound, zoomUpperBound);
}


void Camera::m_UpdateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    cameraFront = glm::normalize(front);
    // also re-calculate the Right and Up vector
    // LookRight();
    // LookUp();
}
