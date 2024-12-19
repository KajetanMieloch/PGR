#pragma once
#include <glad/glad.h>

#include <array>
#include <string>

class Cube {
public:
  enum class Type { None, Grass, Stone, Grass_debug };

  Cube(const std::string &texturePath);
  Cube(Type type = Type::None) : m_type(type) {}

  Type GetType() const { return m_type; }

  Cube() = delete;
  Cube(const Cube &) = delete;
  Cube &operator=(const Cube &) = delete;
  Cube(Cube &&) noexcept;
  Cube &operator=(Cube &&) noexcept;
  ~Cube();

  GLuint Vbo() const { return m_vbo; }
  GLuint Vao() const { return m_vao; }
  void Draw() const;
  GLuint Texture() const { return m_texture; }

private:
  GLuint m_vbo{0};
  GLuint m_vao{0};
  GLuint m_ebo{0};
  GLuint m_texture{0};
  Type m_type;

  static std::array<float, 6 * 6 * 5> s_vertices;

  GLuint CreateTexture(const std::string &texturePath);
};
