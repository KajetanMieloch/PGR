#pragma once
#include "../include/Cube.hpp"
#include "../include/CubePalette.hpp"
#include "../include/ShaderProgram.hpp"
#include "../include/Ray.hpp"
#include "../include/AABB.hpp"

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
  struct HitRecord {
    glm::ivec3 m_cubeIndex;
    glm::ivec3 m_neighbourIndex;
  };

  Chunk(const glm::vec2 &origin, CubePalette &palette);

  void Generate(); // Bez PerlinNoise
  void Draw(ShaderProgram &shader) const;

  Ray::HitType Hit(const Ray &ray, Ray::time_t min, Ray::time_t max, HitRecord &record) const;
  bool RemoveBlock(uint8_t width, uint8_t height, uint8_t depth);

private:
  size_t CoordsToIndex(size_t depth, size_t width, size_t height) const;
  void UpdateVisibility();

  CubePalette &m_palette;
  FlattenData_t m_data;
  glm::vec2 m_origin;
  AABB m_aabb;
};