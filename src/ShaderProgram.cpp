// Modified file for improved clarity (pl/en comments added).


#include "../include/ShaderProgram.hpp"
#include <iostream>

std::string ShaderProgram::s_vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;


    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model *vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    })";

std::string ShaderProgram::s_fragmentShaderSource = R"(

    #version 330 core
    out vec4 FragColor;

    in vec2 TexCoord;

    uniform sampler2D texture1;

    void main() {
        FragColor = texture(texture1, TexCoord);
    })";


GLuint ShaderProgram::createShader(const GLchar *shaderSource,
                                   GLenum shaderType) {
  const GLuint shaderId = glCreateShader(shaderType);
  if (!shaderId)
    return 0;

  glShaderSource(shaderId, 1, &shaderSource, nullptr);
  glCompileShader(shaderId);


  GLint success;
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[512];
    glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
    std::cerr << "ERROR: Shader Compilation Failed\n" << infoLog << std::endl;
    glDeleteShader(shaderId);
    return 0;
  }


  return shaderId;
}

GLuint ShaderProgram::createProgram(GLuint vertexShader, GLuint fragmentShader,
                                    GLuint geometryShader) {
  programId = glCreateProgram();
  if (!programId)
    return 0;

  glAttachShader(programId, vertexShader);

  glAttachShader(programId, fragmentShader);
  if (geometryShader != 0)
    glAttachShader(programId, geometryShader);

  glLinkProgram(programId);
  return programId;
}

std::pair<GLuint, GLuint> ShaderProgram::createVertexBufferObject() {

  const float triangle[] = {//  x          y          z

                            -0.5f, -0.5f, 0.0f, 0.5f, -0.5f,
                            0.0f,  0.0f,  0.5f, 0.0f};

  GLuint vbo, vao;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);


  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return std::make_pair(vbo, vao);
}

ShaderProgram::ShaderProgram() {

  vertexShader = createShader(s_vertexShaderSource.c_str(), GL_VERTEX_SHADER);
  fragmentShader =
      createShader(s_fragmentShaderSource.c_str(), GL_FRAGMENT_SHADER);
  programId = createProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void ShaderProgram::cleanUp(std::pair<GLuint, GLuint> vv) {
  glDeleteVertexArrays(1, &vv.second);

  glDeleteBuffers(1, &vv.first);
  glDeleteProgram(programId);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

ShaderProgram::ShaderProgram(ShaderProgram &&rhs) noexcept
    : programId(std::exchange(rhs.programId, 0)) {}

ShaderProgram &ShaderProgram::operator=(ShaderProgram &&rhs) noexcept {

  if (&rhs == this) {
    return *this;
  }

  programId = std::exchange(rhs.programId, 0);

  return *this;
}

void ShaderProgram::use() { glUseProgram(programId); }


void ShaderProgram::setUniform(const std::string &name,
                               const glm::mat4 &matrix) {
  GLint location = glGetUniformLocation(programId, name.c_str());
  if (location != -1) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
  } else {
    std::cerr << "Uniform '" << name << "' not found in shader program!"
              << std::endl;
  }

}

void ShaderProgram::setMat4(const std::string_view name,
                            const glm::mat4 &value) {
  GLint location = glGetUniformLocation(programId, name.data());
  if (location != -1) {
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
  } else {
    std::cerr << "Uniform '" << name << "' not found in shader program!"
              << std::endl;

  }
}
