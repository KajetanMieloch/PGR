#pragma once
#include "../include/Cube.hpp"
#include "../include/CubePalette.hpp"
#include "../include/ShaderProgram.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <array>

template <uint8_t Depth, uint8_t Width, uint8_t Height>
class Chunk {
  struct CubeData {
    Cube::Type m_type{Cube::Type::None};
    bool m_isVisible{true};
  };

  using FlattenData_t = std::array<CubeData, Depth * Width * Height>;

public:
  Chunk(const glm::vec2 &origin, CubePalette &palette);

  void Generate(); // Bez PerlinNoise

  void Draw(ShaderProgram &shader) const;

private:
  size_t CoordsToIndex(size_t depth, size_t width, size_t height) const;

  void UpdateVisibility();

  CubePalette &m_palette;
  FlattenData_t m_data;
  glm::vec2 m_origin;
};

// Konstruktor
template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline Chunk<Depth, Width, Height>::Chunk(const glm::vec2 &origin,
                                          CubePalette &palette)
    : m_origin(origin), m_palette(palette), m_data{} {}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Generate() {
  for (size_t z = 0; z < Depth; ++z) {
    for (size_t x = 0; x < Width; ++x) {
      for (size_t y = 0; y < Height; ++y) {
        size_t index = CoordsToIndex(z, x, y);

        if (y == Height - 1) {
          m_data[index].m_type = Cube::Type::Grass; // Górna warstwa
        } else {
          m_data[index].m_type = Cube::Type::Stone; // Wnętrze
        }
      }
    }
  }
  UpdateVisibility();
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::Draw(ShaderProgram &shader) const {
  shader.use();

  for (size_t z = 0; z < Depth; ++z) {
    for (size_t x = 0; x < Width; ++x) {
      for (size_t y = 0; y < Height; ++y) {
        size_t index = CoordsToIndex(z, x, y);

        if (m_data[index].m_isVisible) {
          glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
          shader.setMat4("model", model);
          m_palette.LookUp(m_data[index].m_type).Draw();
        }
      }
    }
  }
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline size_t Chunk<Depth, Width, Height>::CoordsToIndex(size_t depth,
                                                         size_t width,
                                                         size_t height) const {
  return height * static_cast<size_t>(Depth) * static_cast<size_t>(Width) +
         width * static_cast<size_t>(Depth) + depth;
}

template <uint8_t Depth, uint8_t Width, uint8_t Height>
inline void Chunk<Depth, Width, Height>::UpdateVisibility() {
  for (size_t z = 0; z < Depth; ++z) {
    for (size_t x = 0; x < Width; ++x) {
      for (size_t y = 0; y < Height; ++y) {
        size_t index = CoordsToIndex(z, x, y);

        if (m_data[index].m_type == Cube::Type::None) {
          m_data[index].m_isVisible = false;
          continue;
        }
        bool isVisible = false;
        if (z == 0 ||
            m_data[CoordsToIndex(z - 1, x, y)].m_type == Cube::Type::None)
          isVisible = true;
        if (z == Depth - 1 ||
            m_data[CoordsToIndex(z + 1, x, y)].m_type == Cube::Type::None)
          isVisible = true;
        if (x == 0 ||
            m_data[CoordsToIndex(z, x - 1, y)].m_type == Cube::Type::None)
          isVisible = true;
        if (x == Width - 1 ||
            m_data[CoordsToIndex(z, x + 1, y)].m_type == Cube::Type::None)
          isVisible = true;
        if (y == 0 ||
            m_data[CoordsToIndex(z, x, y - 1)].m_type == Cube::Type::None)
          isVisible = true;
        if (y == Height - 1 ||
            m_data[CoordsToIndex(z, x, y + 1)].m_type == Cube::Type::None)
          isVisible = true;
        m_data[index].m_isVisible = isVisible;
      }
    }
  }
}
