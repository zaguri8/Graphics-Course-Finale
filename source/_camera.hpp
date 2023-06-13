#ifndef _CAMERA
#define _CAMERA
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <GLFW/glfw3.h>
using namespace glm;
using namespace std;


class Camera {
public:
    Camera(
        float aspect,
        vec3 cameraPosition = vec3(0, 0, 1),
        vec3 cameraFront = vec3(0, 0, -1),
        vec3 cameraUp = vec3(0, 1, 0),
        float FOV = 45.0f,
        float near = 100.0f,
        float far = 0.1f
    );


    mat4 getProjectionMatrix();
    mat4 getViewMatrix();

    void setPosition(vec3 position);
    void setFront(vec3 front);
    void setUp(vec3 up);
    void setFOV(float fov);
    void setNear(float near);
    void setFar(float far);
    void setAspect(float aspect);

    void setYaw(float yaw);
    void setPitch(float pitch);

    const vec3& getPosition() const;
    const vec3& getFront() const;
    const vec3& getUp() const;
    const float getNear() const;
    const float getFar() const;
    const float getAspect() const;
    const float getFOV() const;


    const float getYaw() const;
    const float getPitch() const;

    vec3 getRight();


    /**
     * @brief Updates the camera's direction and right vectors based on its current orientation.
     *
     * This function recalculates the camera's direction and right vectors to align with its updated orientation.
     * It is typically called after the camera's orientation has been modified, such as through rotation.
     * By updating these vectors, the camera can correctly determine its movement and view direction.
     */
    void updateCameraVectors();

    /**
     * @brief Processes user input to control camera movement.
     *
     * This function handles user input events, such as keyboard or mouse movements, from the specified GLFW window.
     * It interprets the input and calculates the appropriate camera movement or changes in the camera's view matrix.
     * The camera's internal state is updated to reflect the changes in movement.
     *
     * @param pWindowHandle The GLFW window handle for input detection.
     */
    void processMovement(GLFWwindow* pWindowHandle);
private:
    vec3 cameraPosition;
    vec3 cameraFront;
    vec3 cameraUp;
    vec3 cameraRight;

    vec3 worldUp = vec3(0, 1, 0);

    float aspect;
    float FOV;
    float near;
    float far;

    float yaw = -90.0f;
    float pitch = 0;
    vec3 angularSpeed = vec3(0.5, 0.5, 0.5); // degrees per second
    vec3 speed = vec3(0.01, 0.01, 0.01); // units per second
};

#endif
