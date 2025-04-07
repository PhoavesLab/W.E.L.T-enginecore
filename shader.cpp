#include "shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace renderer {

    Shader::Shader() : m_id(0) {
    }

    Shader::~Shader() {
        if (m_id != 0) {
            glDeleteProgram(m_id);
        }
    }

    bool Shader::compile(const char* vertexSource, const char* fragmentSource) {
        // Compile vertex shader
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexSource, NULL);
        glCompileShader(vertexShader);

        // Check for vertex shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            return false;
        }

        // Compile fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
        glCompileShader(fragmentShader);

        // Check for fragment shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return false;
        }

        // Link shaders
        m_id = glCreateProgram();
        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);
        glLinkProgram(m_id);

        // Check for linking errors
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_id, 512, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteProgram(m_id);
            m_id = 0;
            return false;
        }

        // Delete shaders as they're linked into our program now and no longer necessary
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return true;
    }

    void Shader::use() {
        glUseProgram(m_id);
    }

    void Shader::setBool(const std::string& name, bool value) {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
    }

    void Shader::setInt(const std::string& name, int value) {
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::setFloat(const std::string& name, float value) {
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
    }

    void Shader::setVec2(const std::string& name, const glm::vec2& value) {
        glUniform2fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec3(const std::string& name, const glm::vec3& value) {
        glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setVec4(const std::string& name, const glm::vec4& value) {
        glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
    }

    void Shader::setMat2(const std::string& name, const glm::mat2& value) {
        glUniformMatrix2fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setMat3(const std::string& name, const glm::mat3& value) {
        glUniformMatrix3fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void Shader::setMat4(const std::string& name, const glm::mat4& value) {
        glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

} // namespace renderer

