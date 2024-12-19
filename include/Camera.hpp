#pragma once
#include <SFML/Window.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>

class Camera {
public:
  Camera(const glm::vec3 &position, const glm::vec3 &front, float yaw,
         float pitch);

  glm::mat4 View() const { return m_lookAt; }
  glm::mat4 Projection() const { return m_projection; }

  void Rotate(const sf::Vector2i &mouseDelta);
  void MoveForward(float dt);
  void MoveBackward(float dt);
  void MoveLeft(float dt);
  void MoveRight(float dt);
  void MoveUp(float dt);
  void MoveDown(float dt);

private:
  void RecreateLookAt();

  glm::mat4 m_projection;
  glm::mat4 m_lookAt;

  glm::vec3 m_position;
  glm::vec3 m_up;
  glm::vec3 m_front;
  glm::vec3 m_right;
  float m_yaw;
  float m_pitch;

  static glm::vec3 s_worldUp;
};
