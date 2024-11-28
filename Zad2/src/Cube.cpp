#include "../include/Cube.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

std::array<float, 6 * 6 * 5> Cube::s_vertices = {
    -0.5f, -0.5f, 0.5f, 0.25f, 1.0f, 0.5f, -0.5f, 0.5f, 0.5f, 1.0f, 0.5f, 0.5f,
    0.5f, 0.5f, 2.0f / 3.0f, 0.5f, 0.5f, 0.5f, 0.5f, 2.0 / 3.0f, -0.5f, 0.5f,
    0.5f, 0.25f, 2.0 / 3.0f, -0.5f, -0.5f, 0.5f, 0.25f, 1.0f,

    -0.5f, -0.5f, -0.5f, 0.5f, 0.0f, 0.5f, -0.5f, -0.5f, 0.25f, 0.0f, 0.5f,
    0.5f, -0.5f, 0.25f, 1.0f / 3.0f, 0.5f, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f,
    -0.5f, 0.5f, -0.5f, 0.25f, 1.0f / 3.0f, -0.5f, -0.5f, -0.5f, 0.25f, 0.0f,

    -0.5f, 0.5f, 0.5f, 0.25f, 2.0f / 3.0f, -0.5f, 0.5f, -0.5f, 0.25f,
    1.0f / 3.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f / 3.0f, -0.5f, -0.5f, -0.5f,
    0.0f, 1.0f / 3.0f, -0.5f, -0.5f, 0.5f, 0.0f, 2.0f / 3.0f, -0.5f, 0.5f, 0.5f,
    0.25f, 2.0f / 3.0f,

    0.5f, 0.5f, 0.5f, 0.5f, 2.0f / 3.0f, 0.5f, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f,
    0.5f, -0.5f, -0.5f, 0.75f, 1.0f / 3.0f, 0.5f, -0.5f, -0.5f, 0.75f,
    1.0f / 3.0f, 0.5f, -0.5f, 0.5f, 0.75f, 2.0f / 3.0f, 0.5f, 0.5f, 0.5f, 0.5f,
    2.0f / 3.0f,

    -0.5f, -0.5f, -0.5f, 0.75f, 2.0f / 3.0f, 0.5f, -0.5f, -0.5f, 0.75f,
    1.0f / 3.0f, 0.5f, -0.5f, 0.5f, 1.0f, 1.0f / 3.0f, 0.5f, -0.5f, 0.5f, 1.0f,
    1.0f / 3.0f, -0.5f, -0.5f, 0.5f, 1.0f, 2.0f / 3.0f, -0.5f, -0.5f, -0.5f,
    0.75f, 2.0f / 3.0f,

    -0.5f, 0.5f, -0.5f, 0.5f, 1.0f / 3.0f, 0.5f, 0.5f, -0.5f, 0.5f, 2.0f / 3.0f,
    0.5f, 0.5f, 0.5f, 0.25f, 2.0f / 3.0f, 0.5f, 0.5f, 0.5f, 0.25f, 2.0f / 3.0f,
    -0.5f, 0.5f, 0.5f, 0.25f, 1.0f / 3.0f, -0.5f, 0.5f, -0.5f, 0.5f,
    1.0f / 3.0f};

Cube::Cube(Cube &&rhs) noexcept
    : m_vbo(std::exchange(rhs.m_vbo, 0)), m_vao(std::exchange(rhs.m_vao, 0)),
      m_texture(std::exchange(rhs.m_texture, 0)) {}

Cube &Cube::operator=(Cube &&rhs) noexcept {
  if (&rhs == this) {
    return *this;
  }

  m_vbo = std::exchange(rhs.m_vbo, 0);
  m_vao = std::exchange(rhs.m_vao, 0);
  m_texture = std::exchange(rhs.m_texture, 0);

  return *this;
}

Cube::Cube(const std::string &texturePath) {
  m_texture = CreateTexture(texturePath);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, s_vertices.size() * sizeof(float),
               s_vertices.data(), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);
}

Cube::~Cube() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
  glDeleteTextures(1, &m_texture);
}

GLuint Cube::CreateTexture(const std::string &texturePath) {
  sf::Image image;
  if (!image.loadFromFile(texturePath)) {
    std::cerr << "Failed to load texture from: " << texturePath << std::endl;
    return 0;
  }


  image.flipVertically();

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,
               image.getSize().x, image.getSize().y,
               0,
               GL_RGBA,
               GL_UNSIGNED_BYTE,
               image.getPixelsPtr()
  );

  // Ustawienia tekstury
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

void Cube::Draw() const {
  glBindVertexArray(m_vao);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glDrawArrays(GL_TRIANGLES, 0,
               36);
  glBindVertexArray(0);
}
