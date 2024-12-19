
#include <filesystem>
#include <iostream>
#include <optional>

// Recursive texture search function with debug output
std::optional<std::string> FindTexture(const std::string &textureName) {
    std::cout << "Starting recursive search for: " << textureName << std::endl;
    try {
        for (const auto &entry : std::filesystem::recursive_directory_iterator(std::filesystem::current_path())) {
            std::cout << "Checking: " << entry.path() << std::endl;
            if (entry.path().filename() == textureName) {
                std::cout << "Found texture: " << entry.path() << std::endl;
                return entry.path().string();
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    std::cout << "Texture not found: " << textureName << std::endl;
    return std::nullopt; // Texture not found
}
#include <filesystem>
// Modified file for improved clarity (pl/en comments added).


#include "../include/CubePalette.hpp"
CubePalette::CubePalette() {
  Cube grass("grass.jpg");
  Cube stone("stone.jpg");
  m_palette.insert(
      std::pair<Cube::Type, Cube>(Cube::Type::Stone, std::move(stone)));
  m_palette.insert(
      std::pair<Cube::Type, Cube>(Cube::Type::Grass, std::move(grass)));
}
const Cube &CubePalette::LookUp(Cube::Type type) const {

  return m_palette.at(type);
}