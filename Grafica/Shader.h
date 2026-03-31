#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;

    Shader(const char* vertexPath, const char* fragmentPath)
    {
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;

        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);

            // FIX #2: typo corectat (fShaderSteam -> fShaderStream)
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = stripBOM(vShaderStream.str());
            fragmentCode = stripBOM(fShaderStream.str());
        }
        catch (std::ifstream::failure& e) {
            std::cout << "EROARE::SHADER::Fisier necitit: " << e.what() << std::endl;
            ID = 0;
            return;
        }

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        int success;
        char infoLog[512];

        // FIX #1: verificare erori compilare vertex shader
        unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "EROARE::SHADER::VERTEX::Compilare esuata:\n" << infoLog << std::endl;
        }

        // FIX #1: verificare erori compilare fragment shader
        unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "EROARE::SHADER::FRAGMENT::Compilare esuata:\n" << infoLog << std::endl;
        }

        // FIX #1: verificare erori linking
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        glLinkProgram(ID);
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(ID, 512, NULL, infoLog);
            std::cout << "EROARE::SHADER::PROGRAM::Linking esuat:\n" << infoLog << std::endl;
            // FIX #3: marcăm ID-ul ca invalid dacă linking-ul eșuează
            glDeleteProgram(ID);
            ID = 0;
        }

        // Shader-ele individuale nu mai sunt necesare după linking
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // FIX #5: destructor — eliberăm programul din GPU
    ~Shader() {
        if (ID != 0)
            glDeleteProgram(ID);
    }

    void use() const {
        if (ID != 0)
            glUseProgram(ID);
    }

    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }

    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }

    // FIX #4: metodă lipsă pentru float
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }

    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }

private:
    // Elimină BOM-ul UTF-8 (0xEF 0xBB 0xBF) dacă există la începutul fișierului
    std::string stripBOM(const std::string& str) const {
        if (str.size() >= 3 &&
            (unsigned char)str[0] == 0xEF &&
            (unsigned char)str[1] == 0xBB &&
            (unsigned char)str[2] == 0xBF)
        {
            return str.substr(3);
        }
        return str;
    }
};

#endif