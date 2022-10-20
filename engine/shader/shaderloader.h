#pragma once

#include <common/common.h>
#include <common/loader.h>
#include <vector>

class ShaderLoader : public ULoader{
public:
    GLuint load(const vector<filesystem::path>& paths) {
        vector<GLuint> shaders;
        for (size_t i = 0; i < paths.size(); ++i) {
            shaders.push_back(compileShader(paths.at(i)));
        }
        return compileProgram(shaders);
    }
private:
    char infoLog[512];
    GLint isNoError;
    string temp = "";
    basic_string<GLchar> src = "";
    ifstream file;

    void terminateCompilation(){
        glfwTerminate();
    }

    GLenum getShaderType(const filesystem::path& path){
        GLenum shaderType;

        if(path.extension().string() == ".vsh"){
            shaderType = GL_VERTEX_SHADER;
        }
        else if(path.extension().string() == ".fsh"){
            shaderType = GL_FRAGMENT_SHADER;
        }
        else{
            throw string("Wrong file format: ") + path.string();
        }

        return shaderType;
    }

    // GL_VERTEX_SHADER GL_FRAGMENT_SHADER
    GLuint compileShader(const filesystem::path& path) {
        GLuint shader;

        temp = "";
        src = "";

        file.open(path);

        if (file.fail()) {
            throw string("Cannot open file: ") + path.string();
        }

        while (getline(file, temp))
            src += temp + "\n";

        const GLchar* rawSrc = src.c_str();
        
        file.close();

        GLDBR(shader, glCreateShader(getShaderType(path)));
        GLDB(glShaderSource(shader, 1, &rawSrc, NULL));
        GLDB(glCompileShader(shader));

        GLDB(glGetShaderiv(shader, GL_COMPILE_STATUS, &isNoError));
        if (!isNoError) {
            GLDB(glGetShaderInfoLog(shader, 512, NULL, infoLog));
            cout << "Error compiling vertex shader" << endl;
            cout << infoLog << endl;
            terminateCompilation();
        }

        return shader;
    }
    GLuint compileProgram(vector<GLuint>& shaders) {
        GLuint program;

        GLDBR(program, glCreateProgram());

        for (size_t i = 0; i < shaders.size(); ++i) {
            GLDB(glAttachShader(program, shaders.at(i)));
        }

        GLDB(glLinkProgram(program));

        GLDB(glGetProgramiv(program, GL_LINK_STATUS, &isNoError));
        if (!isNoError) {
            GLDB(glGetProgramInfoLog(program, 512, NULL, infoLog));
            cout << "Error linking program" << endl;
            cout << infoLog << endl;
            terminateCompilation();
        }

        for (size_t i = 0; i < shaders.size(); ++i) {
            GLDB(glDeleteShader(shaders.at(i)));
        }

        // the program should be set in window while cycle
        glUseProgram(0);

        return program;
    }
};