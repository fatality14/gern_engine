#pragma once

#include <fstream>
#include <iostream>

#include <common/gl.h>
#include <common/glm.h>

#include <SOIL2.h>
#include <flags.h>
#include <vector>

using namespace std;

float someCounter = 0;

#ifdef DEBUG_GL
// clear last error if any and check for new after call
#define GLDB(f)                                                                \
    while (glGetError() != GL_NO_ERROR)                                        \
        ;                                                                      \
    f;                                                                         \
    while (GLenum e = glGetError()) {                                          \
        cout << "Caught OpenGL error: " << hex << e << endl                    \
             << "In line: " << __LINE__ << ", file: " << __FILE__ << endl;     \
        throw string("GLDB error");                                            \
    }
#define GLDBR(r, f)                                                            \
    while (glGetError() != GL_NO_ERROR)                                        \
        ;                                                                      \
    r = f;                                                                     \
    while (GLenum e = glGetError()) {                                          \
        cout << "Caught OpenGL error: " << e << endl                           \
             << "In line: " << __LINE__ << ", file: " << __FILE__ << endl;     \
        throw string("GLDBR error");                                           \
    }
#else
#define GLDB(f) f
#endif

class ICommon {
public:
    virtual ~ICommon() {}
};

// TODO: add notes that camera and mesh classes are just data containers, rename
// classes or smth else
// TODO: make methods to access the objects of built-in classes
