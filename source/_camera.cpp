#include "_camera.hpp"

Camera::Camera(
    float aspect,
    vec3 cameraPosition,
    vec3 cameraFront,
    vec3 cameraUp,
    float FOV,
    float near,
    float far
) : cameraPosition(cameraPosition), cameraFront(cameraFront), cameraUp(cameraUp), aspect(aspect), FOV(FOV), near(near), far(far) {
    updateCameraVectors();
}

mat4 Camera::getProjectionMatrix() { return  perspective(radians(FOV), aspect, near, far); }
mat4 Camera::getViewMatrix() { return lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp); }

void Camera::setPosition(vec3 position) { this->cameraPosition = position; }
void Camera::setFront(vec3 front) { this->cameraFront = front; }
void Camera::setUp(vec3 up) { this->cameraUp = up; }
void Camera::setFOV(float fov) { this->FOV = fov; }
void Camera::setNear(float near) { this->near = near; }
void Camera::setFar(float far) { this->far = far; }
void Camera::setAspect(float aspect) { this->aspect = aspect; }

void Camera::setYaw(float yaw) { this->yaw = yaw; updateCameraVectors(); }
void Camera::setPitch(float pitch) { this->pitch = pitch; updateCameraVectors(); }


const vec3& Camera::getPosition() const { return cameraPosition; }
const vec3& Camera::getFront() const { return cameraFront; }
const vec3& Camera::getUp() const { return cameraUp; }
const float Camera::getNear() const { return near; }
const float Camera::getFar() const { return far; }
const float Camera::getAspect() const { return aspect; }
const float Camera::getFOV() const { return FOV; }

const float Camera::getYaw() const { return yaw; }
const float Camera::getPitch() const { return pitch; }

vec3 Camera::getRight() {
    return abs(cross(cameraUp, cameraFront));
}

void Camera::updateCameraVectors() {
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(radians(this->yaw)) * cos(glm::radians(this->pitch));
    front.y = sin(radians(this->pitch));
    front.z = sin(radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->cameraFront = front;
    // Also re-calculate the Right and Up vector
    this->cameraRight = normalize(cross(this->cameraFront, this->worldUp));
    this->cameraUp = normalize(cross(this->cameraRight, this->cameraFront));
}

void Camera::processMovement(GLFWwindow* pWindowHandle) {

    if (glfwGetKey(pWindowHandle, GLFW_KEY_W) == GLFW_PRESS) {
        setPosition(getPosition() + speed.y * getFront());
    }
    else if (glfwGetKey(pWindowHandle, GLFW_KEY_S) == GLFW_PRESS) {
        setPosition(getPosition() - speed.y * getFront());
    }
    if (glfwGetKey(pWindowHandle, GLFW_KEY_A) == GLFW_PRESS) {
        setPosition(getPosition() - speed.x * getRight());
    }
    else if (glfwGetKey(pWindowHandle, GLFW_KEY_D) == GLFW_PRESS) {
        setPosition(getPosition() + speed.x * getRight());
    }


    if (glfwGetKey(pWindowHandle, GLFW_KEY_R) == GLFW_PRESS) {
        setYaw(getYaw() + angularSpeed.y);
    }
    else  if (glfwGetKey(pWindowHandle, GLFW_KEY_Q) == GLFW_PRESS) {
        setYaw(getYaw() - angularSpeed.y);
    }

    if (glfwGetKey(pWindowHandle, GLFW_KEY_1) == GLFW_PRESS) {
        setPitch(getPitch() + angularSpeed.x);
    }
    else  if (glfwGetKey(pWindowHandle, GLFW_KEY_2) == GLFW_PRESS) {
        setPitch(getPitch() - angularSpeed.x);
    }

}
