#include <glad/glad.h>  // Użycie GLAD jako loadera funkcji OpenGL
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>

// Funkcja do tworzenia shadera
GLuint CreateShader(const GLchar* shaderSource, GLenum shaderType) {
    GLuint shaderId = glCreateShader(shaderType);
    if (!shaderId) {
        std::cerr << "Error creating shader" << std::endl;
        return 0;
    }

    glShaderSource(shaderId, 1, &shaderSource, nullptr);
    glCompileShader(shaderId);

    GLint success;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
        glDeleteShader(shaderId);
        return 0;
    }

    return shaderId;
}

// Funkcja do tworzenia programu shaderowego
GLuint CreateProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint programId = glCreateProgram();
    if (!programId) {
        std::cerr << "Error creating program" << std::endl;
        return 0;
    }

    glAttachShader(programId, vertexShader);
    glAttachShader(programId, fragmentShader);
    glLinkProgram(programId);

    GLint success;
    glGetProgramiv(programId, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programId, 512, nullptr, infoLog);
        std::cerr << "Error linking program: " << infoLog << std::endl;
        glDeleteProgram(programId);
        return 0;
    }

    return programId;
}

int main() {
    // Ustawienia kontekstu OpenGL
    sf::ContextSettings settings;
    settings.majorVersion = 3;
    settings.minorVersion = 3;
    settings.depthBits = 24;

    // Utworzenie okna SFML
    sf::Window window(sf::VideoMode(800, 600), "OpenGL Yellow Triangle", sf::Style::Default, settings);
    window.setActive(true);

    // Inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Ustawienie widoku
    glViewport(0, 0, 800, 600);

    // Tworzenie shaderów
    GLuint vertexShader = CreateShader(
        "#version 330 core\n"
        "layout (location = 0) in vec3 position;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n",
        GL_VERTEX_SHADER
    );

    GLuint fragmentShader = CreateShader(
        "#version 330 core\n"
        "out vec4 fragmentColor;\n"
        "void main() {\n"
        "    fragmentColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
        "}\n",
        GL_FRAGMENT_SHADER
    );

    GLuint shaderProgram = CreateProgram(vertexShader, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Dane wierzchołków trójkąta
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  // Lewy dolny wierzchołek
         0.5f, -0.5f, 0.0f,  // Prawy dolny wierzchołek
         0.0f,  0.5f, 0.0f   // Górny wierzchołek
    };

    // Tworzenie VAO i VBO
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Pętla renderująca
    bool running = true;
    while (running) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                running = false;
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        window.display();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    return 0;
}
