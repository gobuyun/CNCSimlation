#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <QDebug>
#include <QtMath>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
//const float YAW = -90.0f;
//const float PITCH = 0.0f;
//const float SPEED = 2.5f;
//const float SENSITIVITY = 0.1f;
//const float ZOOM = 45.0f;


const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 1.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Center;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom = 1.0f;
    float BaseZoom = 1.0f;
    float Distance;

    // constructor with vectors
    Camera(glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f))
        , MovementSpeed(SPEED)
        , MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Center = center;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    void setBaseZoom(double zoom)
    {
        BaseZoom = zoom;
        Zoom = zoom;
    }

    void setDistance(double distance)
    {
        Distance = distance + 1500;
//        Position = glm::vec3(Center.x, Center.y, Center.z+Distance);
        Position = glm::vec3(0, 0, 1000);
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        auto originMat = glm::lookAt(Position, Position + Front, Up);
        originMat = glm::translate(originMat, relPosition);
        // originMat[3].x += relPosition.x;
        // originMat[3].y += relPosition.y;
        // originMat[3].z += relPosition.z;
        return originMat;
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseRotate(float xoffset, float yoffset, bool constrainPitch = true)
    {
        Yaw -= xoffset;
        Pitch -= yoffset;
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
        // recalculte position
        Position = Center-Front*Distance;
    }

    void ProcessMouseMove(float xoffset, float yoffset)
    {
        float sensitivity = BaseZoom/Zoom;
        sensitivity *= 15;
        xoffset *= sensitivity;
        yoffset *= sensitivity;
        relPosition -= Right*xoffset;
        relPosition -= Up*yoffset;
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        double direction = (yoffset/qAbs(yoffset));
        Zoom *= qPow(0.9, -direction);
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        auto it = glm::radians(Yaw);
        auto it2 = glm::radians(Pitch);
        auto it3 = cos(it);
        auto it4 = cos(it2);

        front.x = glm::cos(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
        front.y = glm::sin(glm::radians(Pitch));
        front.z = glm::sin(glm::radians(Yaw)) * glm::cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }

private:
    glm::vec3 relPosition;
};
#endif
