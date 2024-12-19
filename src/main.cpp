#include "../include/Camera.hpp"
#include "../include/Chunk.hpp"
#include "../include/Cube.hpp"
#include "../include/ShaderProgram.hpp"
#include <SFML/Window.hpp>
#include <SFML/Window/Context.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

int main() {
  sf::ContextSettings contextSettings;
  contextSettings.depthBits = 24;
  contextSettings.stencilBits = 8;
  contextSettings.majorVersion = 3;
  contextSettings.minorVersion = 3;

  sf::Window window(sf::VideoMode(800, 600), "Majnkraft", sf::Style::Default, contextSettings);
  window.setActive(true);
  window.setMouseCursorGrabbed(true);
  window.setMouseCursorVisible(false);

  if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction)) {
    std::cerr << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }

  glViewport(0, 0, static_cast<GLsizei>(window.getSize().x), static_cast<GLsizei>(window.getSize().y));
  glEnable(GL_DEPTH_TEST);

  Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, -1.0f), -90.0f, 0.0f);

  ShaderProgram shaders;
  GLuint programId = shaders.getProgramId();
  if (programId == 0) {
    std::cerr << "Failed to create shader program" << std::endl;
    return -1;
  }

  shaders.use();
  shaders.setUniform("projection", camera.Projection());

  CubePalette palette;
  const size_t chunkSize = 16;
  Chunk<chunkSize, chunkSize, chunkSize> chunk(glm::vec2(0, 0), palette);
  chunk.Generate(); // Wywołanie Generate bez PerlinNoise

  sf::Clock clock;
  sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
  sf::Vector2i mousePosition = sf::Mouse::getPosition();

  while (window.isOpen()) {
    const float dt = clock.restart().asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::Resized) {
        glViewport(0, 0, event.size.width, event.size.height);
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
      camera.MoveForward(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      camera.MoveBackward(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
      camera.MoveLeft(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
      camera.MoveRight(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
      camera.MoveUp(dt);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
      camera.MoveDown(dt);
    }

    const sf::Vector2i newMousePosition = sf::Mouse::getPosition();
    camera.Rotate(newMousePosition - mousePosition);
    mousePosition = newMousePosition;

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders.setUniform("view", camera.View());
    shaders.setUniform("projection", camera.Projection());

    chunk.Draw(shaders);

    window.display();
  }

  return 0;
}
