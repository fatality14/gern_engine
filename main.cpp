#include <iostream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>
#include <vector>

#define GLEW_STATIC

using namespace std;

//TODO:
//check in renderer and some other classes that such objects as perspective, view etc use the same window
//also check other similar cases
//may be solved by making Renderer and Object field private

struct Vertex{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

template<class T>
class List{
public:
    vector<T*> list;
    void push(T& m){
        list.push_back(&m);
    }
    void popByName(string name){
        for(int i = 0; i < list.size(); i++){
            if(list.at(i)->name == name){
                list.erase(list.begin() + i);
            }
        }
    }
    T* getByName(string name){
        for(size_t i = 0; i < list.size(); i++){
            if(list.at(i)->name == name){
                return list.at(i);
            }
        }
    }
    T* at(unsigned int i){
        return list.at(i);
    }
};

class Window{
public:
    GLFWwindow* window;

    int width;
    int height;

    int fbWidth;
    int fbHeight;

    GLenum pmFace;
    GLenum pmMode;

    Window(int width, int height, GLenum pmFace = GL_FRONT_AND_BACK, GLenum pmMode = GL_FILL){
        this->width = width;
        this->height = height;
        fbWidth = width;
        fbHeight = height;
        this->pmFace = pmFace;
        this->pmMode = pmMode;
    }
    ~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }
    void setPolygonMode(GLenum face = GL_FRONT_AND_BACK, GLenum mode = GL_FILL){
        glPolygonMode(face, mode);
    }
    void init(){
        initGLFW();
        initWindow();
        initGLEW();
        setPolygonMode(pmFace, pmMode);
    }
private:
    void initGLFW(){
        glfwInit();

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    }
    void initWindow(){
        window = glfwCreateWindow(width, height, "Window", NULL, NULL);

        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
        glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
        glfwMakeContextCurrent(window);
    }
    void initGLEW(){
        //glViewport(0 ,0, fbWidth, fbHeight);

        //init glew
        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK){
            cout << "Im dead" << endl;
            glfwTerminate();
        }

        //setup glew
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH){
        glViewport(0, 0, fbW, fbH);
    }
};

class Mesh{
public:
    vector<Vertex>* vertices;
    vector<GLuint>* indices;

    unsigned int nVertices;
    unsigned int nIndices;

    string name;

    Mesh(const Vertex* vertices, unsigned int nVertices, GLuint* indices, unsigned int nIndices, string name = "noname"){
        this->vertices = new vector<Vertex>(vertices, vertices + nVertices);
        this->indices = new vector<GLuint>(indices, indices + nIndices);

        this->nVertices = nVertices;
        this->nIndices = nIndices;

        this->name = name;
    }

    void pushVertex(Vertex v){
        vertices->push_back(v);
    }
    void addPolyByIndices(GLuint a, GLuint b, GLuint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};

class MeshList : public List<Mesh>{
public:
    void initCube(){
        Mesh* m;
        Vertex vertices[] = {
            //Postition                           Color                               Texcoords                  Normals
            {glm::vec3(-0.5f, 0.5f, 0.0f),        glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.0f, 1.f),      glm::vec3(0.0f, 0.0f, 1.0f)},//0
            {glm::vec3(-0.5f, -0.5f, 0.0f),       glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 0.f),       glm::vec3(0.0f, 0.0f, 1.0f)},//1
            {glm::vec3(0.5f, -0.5f, 0.0f),        glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 0.f),       glm::vec3(0.0f, 0.0f, 1.0f)},//2

            {glm::vec3(0.5f, -0.5f, 0.0f),        glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.0f, 0.f),      glm::vec3(0.0f, 0.0f, 1.0f)},//2
            {glm::vec3(0.5f, 0.5f, 0.0f),         glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, 0.0f, 1.0f)},//3
            {glm::vec3(-0.5f, 0.5f, 0.0f),        glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(0.0f, 0.0f, 1.0f)},//0

            {glm::vec3(0.5f, 0.5f, 0.0f),         glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(0.0f, 1.f),      glm::vec3(1.0f, 0.0f, 0.0f)},//3
            {glm::vec3(0.5f, -0.5f, 0.0f),        glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(0.f, 0.f),       glm::vec3(1.0f, 0.0f, 0.0f)},//2
            {glm::vec3(0.5f, -0.5f, -1.0f),       glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 0.f),       glm::vec3(1.0f, 0.0f, 0.0f)},//6

            {glm::vec3(0.5f, 0.5f, 0.0f),         glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(0.0f, 1.f),      glm::vec3(1.0f, 0.0f, 0.0f)},//3
            {glm::vec3(0.5f, -0.5f, -1.0f),       glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 0.f),       glm::vec3(1.0f, 0.0f, 0.0f)},//6
            {glm::vec3(0.5f, 0.5f, -1.0f),        glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(1.0f, 0.0f, 0.0f)},//7

            {glm::vec3(-0.5f, 0.5f, 0.0f),        glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//0
            {glm::vec3(-0.5f, -0.5f, -1.0f),      glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 0.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//5
            {glm::vec3(-0.5f, -0.5f, 0.0f),       glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(1.f, 0.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//1

            {glm::vec3(-0.5f, 0.5f, 0.0f),        glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//0
            {glm::vec3(-0.5f, 0.5f, -1.0f),       glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//4
            {glm::vec3(-0.5f, -0.5f, -1.0f),      glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 0.f),       glm::vec3(-1.0f, 0.0f, 0.0f)},//5

            {glm::vec3(-0.5f, -0.5f, 0.0f),       glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//1
            {glm::vec3(-0.5f, -0.5f, -1.0f),      glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 0.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//5
            {glm::vec3(0.5f, -0.5f, 0.0f),        glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//2

            {glm::vec3(0.5f, -0.5f, 0.0f),        glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//2
            {glm::vec3(-0.5f, -0.5f, -1.0f),      glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(0.f, 0.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//5
            {glm::vec3(0.5f, -0.5f, -1.0f),       glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(1.f, 0.f),       glm::vec3(0.0f, -1.0f, 0.0f)},//6

            {glm::vec3(-0.5f, 0.5f, -1.0f),       glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//4
            {glm::vec3(0.5f, -0.5f, -1.0f),       glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(0.f, 0.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//6
            {glm::vec3(-0.5f, -0.5f, -1.0f),      glm::vec3(0.0f, 1.0f, 0.0f),        glm::vec2(1.f, 0.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//5

            {glm::vec3(-0.5f, 0.5f, -1.0f),       glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//4
            {glm::vec3(0.5f, 0.5f, -1.0f),        glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//7
            {glm::vec3(0.5f, -0.5f, -1.0f),       glm::vec3(0.0f, 0.5f, 1.0f),        glm::vec2(0.f, 0.f),       glm::vec3(0.0f, 0.0f, -1.0f)},//6

            {glm::vec3(-0.5f, 0.5f, 0.0f),        glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.0f, 0.f),      glm::vec3(0.0f, 1.0f, 0.0f)},//0
            {glm::vec3(0.5f, 0.5f, 0.0f),         glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(1.f, 0.f),       glm::vec3(0.0f, 1.0f, 0.0f)},//3
            {glm::vec3(-0.5f, 0.5f, -1.0f),       glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(0.0f, 1.0f, 0.0f)},//4

            {glm::vec3(-0.5f, 0.5f, -1.0f),       glm::vec3(1.0f, 0.0f, 0.0f),        glm::vec2(0.f, 1.f),       glm::vec3(0.0f, 1.0f, 0.0f)},//4
            {glm::vec3(0.5f, 0.5f, 0.0f),         glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(1.f, 0.f),       glm::vec3(0.0f, 1.0f, 0.0f)},//3
            {glm::vec3(0.5f, 0.5f, -1.0f),        glm::vec3(1.0f, 1.0f, 0.0f),        glm::vec2(1.f, 1.f),       glm::vec3(0.0f, 1.0f, 0.0f)},//7
        };
        unsigned nVertices = sizeof(vertices) / sizeof(Vertex);

        GLuint indices[] = {};
        unsigned nIndices = sizeof(indices) / sizeof(GLuint);

        m = new Mesh(vertices, nVertices, indices, nIndices, "cube");

        push(*m);
    }
    void initDefaultList(){
        initCube();
    }
};

class Shader{
public:
    GLuint program;
    string name;

    Shader(string vertexPath, string fragmentPath, string name = "noname"){
        this->vertexPath = vertexPath;
        this->fragmentPath = fragmentPath;
        this->name = name;
    }
    ~Shader(){
        glDeleteProgram(program);
    }

    void compileShaders(){
        compileVertex();
        compileFragment();
        compileProgram();

        if (!isCompiled){
            glfwTerminate(); //TODO: replace with throw exception or else
        }
    }
    void bind(){
        glUseProgram(program);
    }
    void unbind(){
        glUseProgram(0);
    }
    void setLayout(string layoutName, int vecSize, size_t offset){
        //we set the format of data reading below
        GLuint attribLoc = glGetAttribLocation(program, layoutName.data());
        glVertexAttribPointer(attribLoc, vecSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offset);
        glEnableVertexAttribArray(attribLoc);
    }
    void setUniformMatrix4fv(string uniformName, const GLfloat* value){
        glUniformMatrix4fv(glGetUniformLocation(program, uniformName.data()), 1, GL_FALSE, value);
    }
    void setUniform3fv(string uniformName, const GLfloat* value){
        glUniform3fv(glGetUniformLocation(program, uniformName.data()), 1, value);
    }
    void setUniform1i(string uniformName, GLint location){
        glUniform1i(glGetUniformLocation(program, uniformName.data()), location);
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
};

class ShaderList : public List<Shader>{
    void pushNew(string vertexPath, string fragmentPath, string name = "noname"){
        list.push_back(new Shader(vertexPath, fragmentPath, name));
    }
};

//maybe add BufferList
class Buffer{
public:
    Mesh* mesh;

    Buffer(Mesh& m){
        mesh = &m;
    }

    void genBuffers(){
        initVAO();
        initVBO();
        initEBO();

        //current vao should be set in while window loop
        unbind();
    }
    void setLayouts(Shader* s){
        //no need to bind shader here
        bind();

        s->setLayout("vertex_position", 3, offsetof(Vertex, position));
        s->setLayout("vertex_color", 3, offsetof(Vertex, color));
        s->setLayout("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        s->setLayout("vertex_normal", 3, offsetof(Vertex, normal));

        unbind();
    }
    void bind(){
        glBindVertexArray(VAO);
    }
    void unbind(){
        glBindVertexArray(0);
    }
    const string& getMeshName(){
        return mesh->name;
    }

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    void initVAO(){
        //init and use vao that works just like vbo and ebo buffers wrapper
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }
    void initVBO(){
        //vbo push all vertices array data to gpu and interpret it like some data array
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mesh->vertices->size(), mesh->vertices->data(), GL_STATIC_DRAW);
    }
    void initEBO(){
        //ebo push all indices array data to gpu and interpret it like element array
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices->size(), mesh->indices->data(), GL_STATIC_DRAW);
    }
};

class Texture{
public:
    GLuint textureId;
    string name; //same as path

    Texture(string path){
        name = path;
    }
    ~Texture(){
        glDeleteTextures(1, &textureId);
    }

    void loadTexture(){
        //load image
        image_width = 0;
        image_height = 0;

        image = SOIL_load_image(name.data(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

        initTexture();

        //free space
        SOIL_free_image_data(image);
    }
private:
    int image_width;
    int image_height;

    unsigned char* image;

    void initTexture(){
        //init and active texture
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        //setup texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        //set texture data and mipmap
        //mipmap is used to compress texture or smthg like this
        if(image){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else{
            cout << "Texture loading failed" << endl;
        }
    }
};

class TextureList : List<Texture>{
public:
    void pushNew(string path){
        Texture* t = new Texture(path);
        t->loadTexture();
        list.push_back(t);
        unbindTextures();
    }
    void bindTextureByIndex(Shader* shader, GLenum GL_TEXTURE_n, GLint textureIndex, string uniformName){
        shader->setUniform1i(uniformName, textureIndex);

        //use texture
        glActiveTexture(GL_TEXTURE_n);
        glBindTexture(GL_TEXTURE_2D, list.at(textureIndex)->textureId);
    }
    void bindTextureByName(Shader* shader, GLenum GL_TEXTURE_n, GLint textureNum, string uniformName, string path){
        shader->setUniform1i(uniformName, textureNum);

        //use texture
        glActiveTexture(GL_TEXTURE_n);
        glBindTexture(GL_TEXTURE_2D, getByName(path)->textureId);
    }
    void unbindTextures(){
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

class MouseListener{
public:
    float dt;
    float currTime;
    float lastTime;

    double lastMouseX;
    double lastMouseY;

    double mouseX;
    double mouseY;

    double mouseOffsetX;
    double mouseOffsetY;

    bool firstMouse;

    MouseListener(Window& w){
        window = &w;

        dt = 0;
        currTime = 0;
        lastTime = 0;

        lastMouseX = 0;
        lastMouseY = 0;

        mouseX = 0;
        mouseY = 0;

        mouseOffsetX = 0;
        mouseOffsetY = 0;

        firstMouse = true;
    }

    void update(){
        updateDt();
        updateInput();
        //cout << dt << " " << mouseOffsetX << " " << mouseOffsetY << endl;
    }
private:
    Window* window;
    void updateDt(){
        currTime = static_cast<float>(glfwGetTime());
        dt = currTime - lastTime;
        lastTime = currTime;
    }
    void updateInput(){
        glfwGetCursorPos(window->window, &mouseX, &mouseY);

        if(firstMouse){
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }

        mouseOffsetX = mouseX - lastMouseX;
        mouseOffsetY = lastMouseY - mouseY;

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
};

class Camera{
public:
    string name;

    glm::vec3 position;
    glm::vec3 up;
    glm::vec3 front;
    glm::vec3 right;

    GLfloat pitch = 0.f;
    GLfloat yaw = -90.f;
    GLfloat roll = 0.f;

    GLfloat movementSpeed = 3.f;
    GLfloat sensitivity = 5.f;

    Camera(string name = "noname"){
        this->name = name;

        position = glm::vec3(0.f, 0.f, 1.f);
        up = glm::vec3(0.f, 1.f, 0.f);
        front = glm::vec3(0.f, 0.f, -1.f);
        right = glm::vec3(0.f);
    }
};

class CameraList : public List<Camera>{
public:
    CameraList(){
        push(*(new Camera("default")));
    }
    Camera* getDefaultCam(){
        return at(0);
    }
};

class View{
public:
    glm::vec3 worldUp;

    glm::mat4 viewMatrix;

    Camera* camera;

    View(Window& w, MouseListener& ml, Camera& c){
        window = &w;
        mouse = &ml;
        camera = &c;

        worldUp = glm::vec3(0.f, 1.f, 0.f);

        viewMatrix = glm::lookAt(camera->position, camera->position + camera->front, camera->up);
    }

    void pushToShader(Shader* s){
        s->setUniformMatrix4fv("ViewMatrix", glm::value_ptr(viewMatrix));
        s->setUniform3fv("cameraPos", glm::value_ptr(camera->position));
    }
    void updateMatrices(){
        //viewMatrix = glm::mat4(1.f);
        updateCameraPosition();
        viewMatrix = glm::lookAt(camera->position, camera->position + camera->front, camera->up);
    }

    //TODO: make user function to start here, (might move to renderer class - bad idea)
    void setWindowEvents(void (*userEvents)(View&)){
        if(glfwGetKey(window->window, GLFW_KEY_A) == GLFW_PRESS){
            camera->position -= camera->right * camera->movementSpeed * mouse->dt;
        }
        if(glfwGetKey(window->window, GLFW_KEY_D) == GLFW_PRESS){
            camera->position += camera->right * camera->movementSpeed * mouse->dt;
        }
        if(glfwGetKey(window->window, GLFW_KEY_Q) == GLFW_PRESS){
            camera->position -= camera->up * camera->movementSpeed * mouse->dt;
        }
        if(glfwGetKey(window->window, GLFW_KEY_E) == GLFW_PRESS){
            camera->position += camera->up * camera->movementSpeed * mouse->dt;
        }
        if(glfwGetKey(window->window, GLFW_KEY_S) == GLFW_PRESS){
            camera->position -= camera->front * camera->movementSpeed * mouse->dt;
        }
        if(glfwGetKey(window->window, GLFW_KEY_W) == GLFW_PRESS){
            camera->position += camera->front * camera->movementSpeed * mouse->dt;
        }

        userEvents(*this);
    }
private:
    Window* window;
    MouseListener* mouse;

    void updateCameraPosition(){
        camera->pitch += static_cast<GLfloat>(mouse->mouseOffsetY) * camera->sensitivity * mouse->dt;
        camera->yaw += static_cast<GLfloat>(mouse->mouseOffsetX) * camera->sensitivity * mouse->dt;

        if(camera->pitch > 80.f)
            camera->pitch = 80.f;
        else if (camera->pitch < -80.f)
            camera->pitch = -80.f;

        if (camera->yaw > 360.f || camera->yaw < -360.f)
            camera->yaw = 0.f;

        camera->front.x = cos(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));
        camera->front.y = sin(glm::radians(camera->pitch));
        camera->front.z = sin(glm::radians(camera->yaw)) * cos(glm::radians(camera->pitch));

        camera->front = glm::normalize(camera->front);
        camera->right = glm::normalize(glm::cross(camera->front, worldUp));
        camera->up = glm::normalize(glm::cross(camera->right, camera->front));
    }
};

class Perspective{
public:
    glm::mat4 projectionMatrix;

    float fov = 90.f;
    float nearPlane = 0.001f;
    float farPlane = 1000.f;

    Perspective(Window& w){
        window = &w;

        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }

    void pushToShader(Shader* s){
        s->setUniformMatrix4fv("ProjectionMatrix", glm::value_ptr(projectionMatrix));
    }
    void updateMatrices(){
        glfwGetFramebufferSize(window->window, &window->fbWidth, &window->fbHeight);
        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }

    //TODO: fill with smth later
    void setWindowEvents(void (*userEvents)(Perspective&)){
        userEvents(*this);
    }
private:
    Window* window;
};

class Position{
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 modelMatrix;

    Position(){
        position = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        modelMatrix = glm::mat4(1.f);
    }

    void pushToShader(Shader* s){
        s->setUniformMatrix4fv("ModelMatrix", glm::value_ptr(modelMatrix));
    }
    void updateMatrices(){
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    }

    void setWindowEvents(Window* w, void (*userEvents)(Position&)){
        if(glfwGetKey(w->window, GLFW_KEY_2) == GLFW_PRESS){
            scale -= 0.01f;
        }
        if(glfwGetKey(w->window, GLFW_KEY_1) == GLFW_PRESS){
            scale += 0.01f;
        }
        if(glfwGetKey(w->window, GLFW_KEY_Z) == GLFW_PRESS){
            rotation.y -= 5.f;
        }
        if(glfwGetKey(w->window, GLFW_KEY_X) == GLFW_PRESS){
            rotation.y += 5.f;
        }

        userEvents(*this);
    }
    void move(float x, float y, float z){
        position.x += x;
        position.y += y;
        position.z += z;
    }
    void moveTo(float x, float y, float z){
        position.x = x;
        position.y = y;
        position.z = z;
    }
    void rotate(float x, float y, float z){
        rotation.x += x;
        rotation.y += y;
        rotation.z += z;
    }
    void scaleTo(float x, float y, float z){
        scale.x = x;
        scale.y = y;
        scale.z = z;
    }
};

class LightSource{
public:
    string name;
    glm::vec3 lightPos;

    LightSource(string name = "noname"){
        this->name = name;
        lightPos = glm::vec3(0.f, 0.f, 2.f);
    }
};

class LightSourceList : public List<LightSource>{
public:
    void pushNew(string name = "noname"){
        push(*(new LightSource(name)));
    }
    void pushToShader(Shader* s, int lightSourceNum, string uniformName){
        LightSource* currentSource = list.at(lightSourceNum);
        s->setUniform3fv(uniformName.data(), glm::value_ptr(currentSource->lightPos));
    }
};

class Object{
public:
    Window* window;
    TextureList* tex;
    Shader* shader;
    Buffer* buffer;
    Position* position;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;

    Object(Window& w, TextureList& t, Shader& s, Buffer& b, Perspective& p, View& v, LightSourceList& lsl){
        window = &w;
        tex = &t;
        shader = &s;
        buffer = &b;
        perspective = &p;
        view = &v;
        lightSources = &lsl;

        position = new Position();
    }

    void draw(void (*drawObjectFunction)(Object&), void (*userEvents)(Position&), bool byIndices = true){
        shader->bind();

        position->updateMatrices();
        position->pushToShader(shader);

        position->setWindowEvents(window, userEvents);

        perspective->updateMatrices();
        perspective->pushToShader(shader);

        view->updateMatrices();
        view->pushToShader(shader);

        drawObjectFunction(*this);

        buffer->bind();

        if (byIndices)
            glDrawElements(GL_TRIANGLES, buffer->mesh->nIndices, GL_UNSIGNED_INT, (void*)0);
        else
            glDrawArrays(GL_TRIANGLES, 0, buffer->mesh->nVertices);

        shader->unbind();
    }
    const string& getDrawMeshName(){
        return buffer->getMeshName();
    }
    void move(float x, float y, float z){
        position->move(x,y,z);
    }
    void moveTo(float x, float y, float z){
        position->moveTo(x,y,z);
    }
    void rotate(float x, float y, float z){
        position->rotate(x,y,z);
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
};

class Objects{
public:
    vector<Object*> list;

    void push(Object& o){
        list.push_back(&o);
    }
    Object* at(unsigned int index){
        return list.at(index);
    }
    Object* getByMeshName(string meshName){
        for(size_t i = 0; i < list.size(); i++){
            if(list.at(i)->getDrawMeshName() == meshName){
                return list.at(i);
            }
        }
    }
};

class Renderer{
public:
    Window* window;
    Perspective* perspective;
    View* view;
    Objects* objects;
    MouseListener* mouse;
    CameraList* camList;

    float r, g, b, a;

    Renderer(){
        window = new Window(700, 1040);
        window->init();

        perspective = new Perspective(*window);
        mouse = new MouseListener(*window);
        camList = new CameraList();
        view = new View(*window, *mouse, *camList->at(0));
        objects = new Objects;
    }
    Renderer(Window& w, Perspective& p, View& v, MouseListener& ml, Objects& o, CameraList* cl = nullptr){
        window = &w;
        perspective = &p;
        view = &v;
        mouse = &ml;
        objects = &o;
        camList = cl;
        if (camList == nullptr){
            camList = new CameraList;
        }
    }

    void addNewObject(TextureList& tl, Shader& s, Buffer& b, LightSourceList& lsl){
        Object* o = new Object(*window, tl, s, b, *perspective, *view, lsl);
        objects->push(*o);
    }
    Object* getObjectByIndex(unsigned int index){
        return  objects->at(index);
    }
    void render(void (*frameFunction)(Renderer&),
                void (*userViewEvents)(View&),
                void (*userPerspectiveEvents)(Perspective&)){
        doContinue = true;
        while (!glfwWindowShouldClose(window->window))
        {
            //used to catch events
            glfwPollEvents();

            //get enter press event to fast close
            if(glfwGetKey(window->window, GLFW_KEY_ENTER) == GLFW_PRESS){
                doContinue = false;
            }

            //set background colour of window
            glClearColor(r, g, b, a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            mouse->update();
            perspective->setWindowEvents(userPerspectiveEvents);
            view->setWindowEvents(userViewEvents);

            frameFunction(*this);

            glfwSwapBuffers(window->window);
            glFlush();

            if(!doContinue){
                glfwSetWindowShouldClose(window->window, GLFW_TRUE);
                break;
            }
        }
    }
    void setBackgroundColor(float r, float g, float b, float a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
private:
    bool doContinue = false;
};

void viewEvents(View& v){}
void perspectiveEvents(Perspective& p){}
void positionEvents(Position& p){}

void drawObject(Object& o){
    o.tex->bindTextureByIndex(o.shader, GL_TEXTURE0, 0, "texture0");
    o.tex->bindTextureByIndex(o.shader, GL_TEXTURE1, 1, "specularTex");

    o.lightSources->pushToShader(o.shader, 0, "lightPos0");
}

void drawFrame(Renderer& r){
    Object* currObj = r.objects->list.at(0);
    currObj->draw(drawObject, positionEvents, false);

    currObj = r.objects->list.at(1);
    currObj->draw(drawObject, positionEvents, false);
}

int main (){
    Renderer renderer;

    MeshList meshList;
    meshList.initDefaultList();

    Shader shader("C:\\EngPathReq\\might_beeeeeeeeeeee\\vertex.vsh", "C:\\EngPathReq\\might_beeeeeeeeeeee\\fragment.fsh");
    shader.compileShaders();

    TextureList tex;
    tex.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/box.png");
    tex.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/box1.png");

    LightSource light0("default");
    LightSourceList lightSources;
    lightSources.push(light0);

    Buffer buffer(*meshList.list.at(0));
    buffer.genBuffers();
    buffer.setLayouts(&shader);

    renderer.addNewObject(tex, shader, buffer, lightSources);
    renderer.addNewObject(tex, shader, buffer, lightSources);
    renderer.getObjectByIndex(1)->move(2.f,0,0);

    renderer.setBackgroundColor(0.5f, 0.f, 0.f, 0.99f);
    renderer.render(drawFrame, viewEvents, perspectiveEvents);
    return 0;
}
