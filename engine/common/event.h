#pragma once

#define setEvent(GLuint_window, Button, Code)                                  \
    if (glfwGetKey(GLuint_window, GLFW_KEY_##Button) == GLFW_PRESS) {          \
        Code;                                                                  \
    }
