#include "../include/Camera.hpp"

glm::vec3 Camera::s_worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

Camera::Camera(const glm::vec3 &position, const glm::vec3 &front, float yaw, float pitch)
    : m_position(position), m_front(front), m_yaw(yaw), m_pitch(pitch) {
  RecreateLookAt();
  m_projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
}

void Camera::RecreateLookAt() {
  m_front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front.y = sin(glm::radians(m_pitch));
  m_front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  m_front = glm::normalize(m_front);
  m_right = glm::normalize(glm::cross(m_front, s_worldUp));
  m_up = glm::normalize(glm::cross(m_right, m_front));
  m_lookAt = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::MoveForward(float dt) {
  m_position += m_front * dt;
  RecreateLookAt();
}

void Camera::MoveBackward(float dt) {
  m_position -= m_front * dt;
  RecreateLookAt();
}

void Camera::MoveLeft(float dt) {
  m_position -= m_right * dt;
  RecreateLookAt();
}

void Camera::MoveRight(float dt) {
  m_position += m_right * dt;
  RecreateLookAt();
}

void Camera::MoveUp(float dt) {
  m_position += m_up * dt;
  RecreateLookAt();
}

void Camera::MoveDown(float dt) {
  m_position -= m_up * dt;
  RecreateLookAt();
}

void Camera::Rotate(const sf::Vector2i &mouseDelta) {
  m_yaw += mouseDelta.x;
  m_pitch -= mouseDelta.y;
  m_pitch = std::min(std::max(m_pitch, -89.0f), 89.0f);
  RecreateLookAt();
}
