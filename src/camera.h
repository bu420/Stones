#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    glm::vec3 pos, front, right, up;
    float yaw, pitch;

    void setup(const glm::vec3& pos) {
        this->pos = pos;
        yaw = pitch = 0;
    }

    glm::mat4 view() const {
        return glm::lookAt(pos, pos + front, glm::vec3(0, 1, 0));
    }

    void moveTo(const glm::vec3& pos) {
        this->pos = pos;
    }

    void moveX(float amount) {
        pos += right * amount;
    }

    void moveY(float amount) {
        pos += glm::vec3(0, 1, 0) * amount;
    }

    void moveZ(float amount) {
        pos += glm::cross(right, glm::vec3(0, 1, 0)) * amount;
    }

    void rotate(float yaw, float pitch) {
        this->yaw += yaw;
        this->pitch += pitch;

        if (pitch != 0) {
            if (pitch > 89.99f) {
                this->pitch = 89.99f;
            }
            else if (pitch < -89.99f) {
                this->pitch = -89.99f;
            }
        }
    }

    void update() {
        front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        front.y = std::sin(glm::radians(pitch));
        front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
        front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));
        up = glm::normalize(glm::cross(right, front));
    }
};