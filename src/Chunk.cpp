#include "../include/Chunk.hpp"
#include <iostream>

// Konstruktor
template <uint8_t Depth, uint8_t Width, uint8_t Height>
Chunk<Depth, Width, Height>::Chunk(const glm::vec2 &origin, CubePalette &palette)
    : m_origin(origin), m_palette(palette), m_aabb(
        glm::vec3(origin.x, 0, origin.y),
        glm::vec3(origin.x + Width, Height, origin.y + Depth)) {}

// Generowanie chunk'a
template <uint8_t Depth, uint8_t Width, uint8_t Height>
void Chunk<Depth, Width, Height>::Generate() {
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

// Rysowanie chunk'a
template <uint8_t Depth, uint8_t Width, uint8_t Height>
void Chunk<Depth, Width, Height>::Draw(ShaderProgram &shader) const {
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

// Metoda Hit
template <uint8_t Depth, uint8_t Width, uint8_t Height>
Ray::HitType Chunk<Depth, Width, Height>::Hit(const Ray& ray, Ray::time_t min, Ray::time_t max, HitRecord& record) const {
    AABB::HitRecord chunkRecord;
    if (m_aabb.Hit(ray, min, max, chunkRecord) == Ray::HitType::Miss) {
        return Ray::HitType::Miss;
    }

    glm::vec3 chunkOffset = glm::vec3(m_origin.x, 0, m_origin.y);

    Ray::time_t closestTime = max;
    bool hitDetected = false;

    for (size_t z = 0; z < Depth; ++z) {
        for (size_t x = 0; x < Width; ++x) {
            for (size_t y = 0; y < Height; ++y) {
                size_t index = CoordsToIndex(z, x, y);
                if (!m_data[index].m_isVisible || m_data[index].m_type == Cube::Type::None) {
                    continue;
                }

                glm::vec3 cubeMin = chunkOffset + glm::vec3(x, y, z);
                glm::vec3 cubeMax = cubeMin + glm::vec3(1.0f); // Cubes are 1x1x1
                AABB cubeAABB(cubeMin, cubeMax);

                AABB::HitRecord cubeRecord;
                if (cubeAABB.Hit(ray, min, closestTime, cubeRecord) == Ray::HitType::Hit) {
                    closestTime = cubeRecord.m_time;
                    record.m_cubeIndex = glm::ivec3(z, x, y);

                    glm::ivec3 neighborOffset = glm::ivec3(0);
                    if (cubeRecord.m_axis == AABB::Axis::x) {
                        neighborOffset.x = (ray.Direction().x > 0) ? -1 : 1;
                    } else if (cubeRecord.m_axis == AABB::Axis::y) {
                        neighborOffset.y = (ray.Direction().y > 0) ? -1 : 1;
                    } else if (cubeRecord.m_axis == AABB::Axis::z) {
                        neighborOffset.z = (ray.Direction().z > 0) ? -1 : 1;
                    }
                    record.m_neighbourIndex = record.m_cubeIndex + neighborOffset;

                    hitDetected = true;
                }
            }
        }
    }

    return hitDetected ? Ray::HitType::Hit : Ray::HitType::Miss;
}
// Metoda CoordsToIndex
template <uint8_t Depth, uint8_t Width, uint8_t Height>
size_t Chunk<Depth, Width, Height>::CoordsToIndex(size_t depth, size_t width, size_t height) const {
  return height * static_cast<size_t>(Depth) * static_cast<size_t>(Width) +
         width * static_cast<size_t>(Depth) + depth;
}

// Metoda UpdateVisibility
template <uint8_t Depth, uint8_t Width, uint8_t Height>
void Chunk<Depth, Width, Height>::UpdateVisibility() {
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

// Metoda RemoveBlock
template <uint8_t Depth, uint8_t Width, uint8_t Height>
bool Chunk<Depth, Width, Height>::RemoveBlock(uint8_t width, uint8_t height, uint8_t depth) {
    size_t index = CoordsToIndex(depth, width, height);
    if (m_data[index].m_type == Cube::Type::None)
        return false;
    m_data[index].m_type = Cube::Type::None;
    UpdateVisibility();
    return true;
}

// Eksportowanie szablonów
template class Chunk<16, 16, 16>;