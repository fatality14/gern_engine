#pragma once

#include "fwd.hpp"
#include <common/alist.h>
#include <common/common.h>
#include <string>
#include <utility>
#include <vector>

class Shader : public ICommon {
public:
    GLuint program;
    string name;

    Shader() = default;
    Shader(GLuint program, const string& name = "noname") {
        this->program = program;
        this->name = name;
    }
    Shader(Shader& s) = delete;
    ~Shader() { GLDB(glDeleteProgram(program)); }

    void bind() { GLDB(glUseProgram(program)); }
    void unbind() {
        // no need for GLDB here, cause unbind is undefined in GL specs
        glUseProgram(0);
    }

    template <class T>
    void setFloatAttribPointer(const string& layoutName, GLint vecSize,
                               size_t offset) {
        GLuint attribLoc;
        GLDBR(attribLoc, glGetAttribLocation(program, layoutName.data()));
        if (attribLoc == (GLuint)-1) {
            cout << "Warning: attribute location in shader " << name
                 << " starts with gl_ or undefined, "
                 << "layout is not set\n";
            return;
        }

        GLDB(glVertexAttribPointer(attribLoc, vecSize, GL_FLOAT, GL_FALSE,
                                   sizeof(T), (GLvoid*)offset));
        GLDB(glEnableVertexAttribArray(attribLoc));
    }
    template <class T>
    void setIntAttribPointer(const string& layoutName, GLint vecSize, size_t offset) {
        GLuint attribLoc;
        GLDBR(attribLoc, glGetAttribLocation(program, layoutName.data()));
        if (attribLoc == (GLuint)-1) {
            cout << "Warning: attribute location in shader " << name
                 << " starts with gl_ or undefined, "
                 << "layout is not set\n";
            return;
        }

        GLDB(glVertexAttribPointer(attribLoc, vecSize, GL_INT, GL_FALSE,
                                   sizeof(T), (GLvoid*)offset));
        GLDB(glEnableVertexAttribArray(attribLoc));
    }
    template <class T>
    void setMatAttribPointer(const string& layoutName, GLint matSize, size_t offset,
                             GLuint divisor = 0) {
        GLuint attribLoc;
        GLDBR(attribLoc, glGetAttribLocation(program, layoutName.data()));

        if (attribLoc == (GLuint)-1) {
            cout << "Warning: attribute location in shader " << name
                 << " starts with gl_ or undefined, "
                 << "layout is not set\n";
            return;
        }

        for (GLint i = 0; i < matSize; ++i) {
            GLDB(glVertexAttribPointer(
                attribLoc + i, matSize, GL_FLOAT, GL_FALSE, sizeof(T),
                (GLvoid*)(offset + sizeof(GLfloat) * matSize * i)));
            GLDB(glEnableVertexAttribArray(attribLoc + i));
        }

        // the divisor by default is set by 0 by ogl
        if (divisor != 0) {
            for (GLint i = 0; i < matSize; ++i) {
                GLDB(glVertexAttribDivisor(attribLoc + i, divisor));
            }
        }
    }

    //TODO refactor value argument
    void setUniformMatrix4fv(const string& uniformName, const GLfloat* const value,
                             GLsizei size = 1) {
        GLDB(glUniformMatrix4fv(
            glGetUniformLocation(program, uniformName.data()), size, GL_FALSE,
            value));
    }
    //TODO refactor value argument
    void setUniform3fv(const string& uniformName, const GLfloat* const value,
                       GLsizei size = 1) {
        GLDB(glUniform3fv(glGetUniformLocation(program, uniformName.data()),
                          size, value));
    }
    void setUniform1i(const string& uniformName, GLint value) {
        GLDB(glUniform1i(glGetUniformLocation(program, uniformName.data()),
                         value));
    }
    void setUniform1f(const string& uniformName, GLfloat value) {
        GLDB(glUniform1f(glGetUniformLocation(program, uniformName.data()),
                         value));
    }
};

class ShaderList : public AListO<Shader> {

};
