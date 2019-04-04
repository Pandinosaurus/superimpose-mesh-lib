/*
 * Copyright (C) 2016-2019 Istituto Italiano di Tecnologia (IIT)
 *
 * This software may be modified and distributed under the terms of the
 * BSD 3-Clause license. See the accompanying LICENSE file for details.
 */

#include "SuperimposeMesh/Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
{
    /* Retrieve the vertex/fragment source code from filePath. */
    std::string   vertexCode;
    std::string   fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    /* Ensures ifstream objects can throw exceptions */
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);

    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream;
        std::stringstream fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (const std::ifstream::failure& e)
    {
        throw std::runtime_error("ERROR::IFSTREAM::FAILURE\n" + std::string(e.what()) +
                                 "\nERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();

    /* Compile shaders. */
    GLuint vertex;
    GLuint fragment;
    GLint success;
    GLchar infoLog[512];

    /* Vertex Shader. */
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    /* Print compile errors if any. */
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        throw std::runtime_error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" + std::string(infoLog));
    };

    /* Fragment Shader. */
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    /* Print compile errors if any. */
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        throw std::runtime_error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" + std::string(infoLog));
    };

    /* Shader Program. */
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    glLinkProgram(this->Program);

    /* Print linking errors if any. */
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        throw std::runtime_error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n" + std::string(infoLog));
    }

    /* Delete the shaders as they're linked into our program now and no longer necessery. */
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}


void Shader::install()
{
    glUseProgram(this->Program);
}


void Shader::uninstall()
{
    glUseProgram(0);
}
