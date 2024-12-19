#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <utility>

class ShaderProgram {
public:
  ShaderProgram();
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram &operator=(const ShaderProgram &) = delete;
  ShaderProgram(ShaderProgram &&rhs) noexcept;
  ShaderProgram &operator=(ShaderProgram &&rhs) noexcept;
  // ~ShaderProgram();

  void use();

  void setInt(const std::string_view name, int value);
  void setMat4(const std::string_view name, const glm::mat4 &value);

  GLuint getProgramId() const { return programId; };
  std::pair<GLuint, GLuint> createVertexBufferObject();
  void cleanUp(std::pair<GLuint, GLuint> vv);
  void setUniform(const std::string &name, const glm::mat4 &matrix);

private:
  GLuint createShader(const GLchar *shaderSource, GLenum shaderType);
  GLuint createProgram(GLuint vertexShader, GLuint fragmentShader,
                       GLuint geometryShader = 0);
  GLuint programId{};
  GLuint vertexShader;
  GLuint fragmentShader;

  static std::string s_vertexShaderSource;
  static std::string s_fragmentShaderSource;
};