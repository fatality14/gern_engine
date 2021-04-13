#pragma once

#include <engine/common.h>

class Shader{
public:
    GLuint program;
    string name;

    Shader(string vertexPath, string fragmentPath, string name = "noname"){
        this->vertexPath = vertexPath;
        this->fragmentPath = fragmentPath;
        this->name = name;

        compileShaders();
    }
    Shader(Shader& s) = delete;
    ~Shader(){
        glDeleteProgram(program);
    }

    void bind(){
        glUseProgram(program);
    }
    void unbind(){
        glUseProgram(0);
    }

    template<class T>
    void setFloatAttribPointer(string layoutName, int vecSize, size_t offset){
        //set the format of data reading
        GLuint attribLoc = glGetAttribLocation(program, layoutName.data());
        glVertexAttribPointer(attribLoc, vecSize, GL_FLOAT, GL_FALSE, sizeof(T), (GLvoid*)offset);
        glEnableVertexAttribArray(attribLoc);
    }
    template<class T>
    void setIntAttribPointer(string layoutName, int vecSize, size_t offset){
        //set the format of data reading
        GLuint attribLoc = glGetAttribLocation(program, layoutName.data());
        glVertexAttribPointer(attribLoc, vecSize, GL_INT, GL_FALSE, sizeof(T), (GLvoid*)offset);
        glEnableVertexAttribArray(attribLoc);
    }
    template<class T>
    void setMatAttribPointer(string layoutName, int matSize, size_t offset, size_t divisor = 0){
        //set the format of data reading
        GLuint attribLoc = glGetAttribLocation(program, layoutName.data());
        for(int i = 0; i < matSize; ++i){
            glVertexAttribPointer(attribLoc + i,
                                  matSize,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(T),
                                  (GLvoid*)(offset + sizeof(float) * matSize * i));
            glEnableVertexAttribArray(attribLoc + i);
        }

        //the divisor by default is set by 0 by ogl
        if(divisor != 0){
            for(int i = 0; i < matSize; ++i){
                glVertexAttribDivisor(attribLoc, divisor);
            }
        }
    }

    void setUniformMatrix4fv(string uniformName, const GLfloat* value, size_t size = 1){
        glUniformMatrix4fv(glGetUniformLocation(program, uniformName.data()), size, GL_FALSE, value);
    }
    void setUniform3fv(string uniformName, const GLfloat* value, size_t size = 1){
        glUniform3fv(glGetUniformLocation(program, uniformName.data()), size, value);
    }
    void setUniform1i(string uniformName, GLint value){
        glUniform1i(glGetUniformLocation(program, uniformName.data()), value);
    }
    void setUniform1f(string uniformName, GLfloat value){
        glUniform1f(glGetUniformLocation(program, uniformName.data()), value);
    }
private:
    GLuint vertexShader;
    GLuint fragmentShader;
    string vertexPath;
    string fragmentPath;
    bool isCompiled = true;
    char infoLog[512];
    GLint isError;
    string temp = "";
    string src = "";
    ifstream in_file;
    const GLchar* vertSrc;
    const GLchar* fragSrc;

    void compileVertex(){
        temp = "";
        src = "";

        //read shader file
        in_file.open(vertexPath);
        if(in_file.is_open()){
            while(getline(in_file, temp))
                src += temp + "\n";
        }
        else {
            cout << "Error reading vertex shader, file not exist" << endl;
            isCompiled = false;
        }
        in_file.close();

        //make vertex shader
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        vertSrc = src.c_str();

        //set shader source and compile
        glShaderSource(vertexShader, 1, &vertSrc, NULL);
        glCompileShader(vertexShader);

        //get compile errors if some
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isError);
        if(!isError){
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            cout << "Error compiling vertex shader" << endl;
            cout << infoLog << endl;
            isCompiled = false;
        }
    }
    void compileFragment(){
        //fragment shader code is the same as vertex
        temp = "";
        src = "";

        in_file.open(fragmentPath);
        if(in_file.is_open()){
            while(getline(in_file, temp))
                src += temp + "\n";
        }
        else {
            cout << "Error reading vertex shader, file not exist" << endl;
            isCompiled = false;
        }
        in_file.close();

        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        fragSrc = src.c_str();

        glShaderSource(fragmentShader, 1, &fragSrc, NULL);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isError);
        if(!isError){
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "Error compiling vertex shader" << endl;
            cout << infoLog << endl;
            isCompiled = false;
        }
    }
    void compileProgram(){
        //init program
        program = glCreateProgram();

        //set compiled shaders to our program
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);

        //compile program
        glLinkProgram(program);

        //get compile errors if some
        glGetProgramiv(program, GL_LINK_STATUS, &isError);
        if(!isError){
            glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
            cout << "Error linking program" << endl;
            cout << infoLog << endl;
            isCompiled = false;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        //the program should be set in window while cycle
        unbind();
    }
    void compileShaders(){
        compileVertex();
        compileFragment();
        compileProgram();

        if (!isCompiled){
            glfwTerminate(); //TODO: replace with throw exception or else
        }
    }
};

class ShaderList : public List<Shader>{
public:
    void pushNew(string vertexPath, string fragmentPath, string name = "noname"){
        Shader* shader = new Shader(vertexPath, fragmentPath, name);
        push(*shader);
    }
};
