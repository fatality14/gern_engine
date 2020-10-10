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

struct Vertex{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

class Window{
public:
    int width;
    int height;
    int fbWidth;
    int fbHeight;
    GLFWwindow* window;
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
    void setPolygonMode(GLenum face = GL_FRONT_AND_BACK, GLenum mode = GL_FILL){
        glPolygonMode(face, mode);
    }
    void init(){
        initGLFW();
        initWindow();
        initGLEW();
        setPolygonMode(pmFace, pmMode);
    }

    static void framebufferResizeCallback(GLFWwindow* window, int fbW, int fbH){
        glViewport(0, 0, fbW, fbH);
    }
};

class Model{
public:
    vector<Vertex>* vertices;
    vector<GLuint>* indices;

    unsigned int nVertices;
    unsigned int nIndices;

    string name;

    Model(){

    }
    Model(const Vertex* vertices, unsigned int nVertices, GLuint* indices, unsigned int nIndices, string name = "noname"){
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

class ModelList{
public:
    vector<Model> list;
    ModelList(){

    }
    void pushModel(Model m){
        list.push_back(m);
    }
    void popModelByName(string name){
        for(int i = 0; i < list.size(); i++){
            if(list.at(i).name == name){
                list.erase(list.begin() + i);
            }
        }
    }
    void initCube(){
        Model* m;
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

        m = new Model(vertices, nVertices, indices, nIndices, "cube");
        list.push_back(*m);
    }
    void initDefaultList(){
        initCube();
    }
};

class Shader{
public:
    GLuint program;
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

    Shader(){

    }
    Shader(string vertexPath, string fragmentPath){
        this->vertexPath = vertexPath;
        this->fragmentPath = fragmentPath;
    }
    ~Shader(){
        glDeleteProgram(program);
    }

    //TODO: divide to smaller chunks
    void compileShaders(){
        compileVertex();
        compileFragment();
        compileProgram();

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        if (!isCompiled){
            glfwTerminate(); //TODO: replace with throw exception or else
        }
    }
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

        //the program should be set in window while cycle
        unuse();
    }
    void use(){
        glUseProgram(program);
    }
    void unuse(){
        glUseProgram(0);
    }
    void setLayout(string layoutName, int vecSize, size_t offset){
        //we set the format of data reading below
        //3 floats to position, 3 to color and 2 to texcoord
        //get location of vertex_position variable from shaders (= 0 for vertex_position)
        GLuint attribLoc = glGetAttribLocation(program, layoutName.data());
        //3 is the amount of data in vec3
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
};

class Buffer{
public:
    Model* model;
    Shader* shader;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    Buffer(Model& m, Shader& s){
        model = &m;
        shader = &s;

        genBuffers();
    }
    ~Buffer(){

    }

    //TODO: divide to smaller chunks
    //TODO: also move glVertexAttribPointer to other class or smth
    void genBuffers(){
        initVAO();
        initVBO();
        initEBO();

        shader->setLayout("vertex_position", 3, offsetof(Vertex, position));
        shader->setLayout("vertex_color", 3, offsetof(Vertex, color));
        shader->setLayout("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setLayout("vertex_normal", 3, offsetof(Vertex, normal));

        //current vao should be set in while window loop
        glBindVertexArray(0);
    }
    void initVAO(){
        //init and use vao that works just like vbo and ebo buffers wrapper
        glCreateVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
    }
    void initVBO(){
        //vbo push all vertices array data to gpu and interpret it like some data array
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model->vertices->size(), model->vertices->data(), GL_STATIC_DRAW);
    }
    void initEBO(){
        //ebo push all indices array data to gpu and interpret it like element array
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * model->indices->size(), model->indices->data(), GL_STATIC_DRAW);
    }
    void bind(){
        glBindVertexArray(VAO);
    }
    void unbind(){
        glBindVertexArray(0);
    }
};

class TextureList{
public:
    vector<GLuint> textures;
    TextureList(){

    }
    void loadTexture(string imagePath){
        //load image
        int image_width = 0;
        int image_height = 0;
        unsigned char* image = SOIL_load_image(imagePath.data(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

        //init and active texture
        textures.push_back(GLuint());
        GLuint& texture = textures.at(textures.size()-1);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

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

        //the texture should be set in while window loop, not here
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        //free space
        SOIL_free_image_data(image);
    }
};

class Perspective{
public:
    Shader* shader;
    Window* window;

    glm::vec3 camPosition;
    glm::vec3 worlUp;
    glm::vec3 camFront;
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    float fov = 90.f;
    float nearPlane = 0.1f;
    float farPlane = 1000.f;

    Perspective(Shader& s, Window& w){
        shader = &s;
        window = &w;

        camPosition = glm::vec3(0.f, 0.f, 1.f);
        worlUp = glm::vec3(0.f, 1.f, 0.f);
        camFront = glm::vec3(0.f, 0.f, -1.f);
        viewMatrix = glm::mat4(1.f);

        viewMatrix = glm::lookAt(camPosition, camPosition + camFront, worlUp);

        projectionMatrix = glm::mat4(1.f);
        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }
    void activate(){
        shader->use();

        shader->setUniformMatrix4fv("ViewMatrix", glm::value_ptr(viewMatrix));
        shader->setUniformMatrix4fv("ProjectionMatrix", glm::value_ptr(projectionMatrix));

        shader->setUniform3fv("cameraPos", glm::value_ptr(camPosition));

        shader->unuse();
    }
    void update(){
        glfwGetFramebufferSize(window->window, &window->fbWidth, &window->fbHeight);
        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }
};

class Position{
public:
    Shader* shader;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 modelMatrix;

    //ADD LIGHT SOURCE CLASS
    glm::vec3 lightPos0;

    Position(Shader& s){
        shader = &s;

        position = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        modelMatrix = glm::mat4(1.f);


        lightPos0 = glm::vec3(0.f,0.f, 2.f);
    }
    void activate(){
        shader->use();

        shader->setUniformMatrix4fv("ModelMatrix", glm::value_ptr(modelMatrix));
        shader->setUniform3fv("lightPos0", glm::value_ptr(lightPos0));

        shader->unuse();
    }
    void update(){
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    }
};

class ModelDrawer{
public:
    TextureList* texList;
    Shader* shader;
    Model* model;
    Buffer* buffer;

    ModelDrawer(TextureList& tl, Shader& s, Model& m){
        texList = &tl;
        buffer = new Buffer(m, s);
    }
    void
};

class Renderer{
public:
    ModelList* models;
    Window* window;
    Shader* shader;
    Buffer* buffer;
    TextureList* texList;
    Position* view;
    Perspective* perspective;

    float r, g, b, a;

    Renderer(ModelList& ml, Window& w, Shader& s, Buffer& b, TextureList& tl, Position& v, Perspective& p){
        models = &ml;
        window = &w;
        shader = &s;
        buffer = &b;
        texList = &tl;
        view = &v;
        perspective = &p;
    }
    ~Renderer(){

    }

    bool doContinue = false;
    void render(void (*frameFunction)(ModelList&, Window&, Shader&, Buffer&, TextureList&, Position&, Perspective&)){
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

            frameFunction(*models, *window, *shader, *buffer, *texList, *view, *perspective);

            glfwSwapBuffers(window->window);
            glFlush();

            if(!doContinue){
                glfwSetWindowShouldClose(window->window, GLFW_TRUE);
                break;
            }
        }
    }
    void stopRendering(){
        doContinue = false;
    }
    void setBackgroundColor(float r, float g, float b, float a){
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

void drawFrame(ModelList& models, Window& window, Shader& shader, Buffer& buffer, TextureList& texList, Position& position, Perspective& perspective){
    //use shaders program
    shader.use();
    //set texture0 to uniform in fragment shader
    //WRAP TO BUFFERLIST WHICH WOULD DRAW ALL MODELS WITH SPECIFIED TEXTURE AND SHADER
    glUniform1i(glGetUniformLocation(shader.program, "texture0"), 0);
    glUniform1i(glGetUniformLocation(shader.program, "specularTex"), 1);

    //use texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texList.textures.at(0));
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texList.textures.at(1));


    glUniformMatrix4fv(glGetUniformLocation(shader.program, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(position.modelMatrix));

    position.update();
    perspective.update();

    if(glfwGetKey(window.window, GLFW_KEY_2) == GLFW_PRESS){
        position.scale -= 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_1) == GLFW_PRESS){
        position.scale += 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_A) == GLFW_PRESS){
        position.position.x += 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_D) == GLFW_PRESS){
        position.position.x -= 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_Q) == GLFW_PRESS){
        position.position.y += 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_E) == GLFW_PRESS){
        position.position.y -= 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_S) == GLFW_PRESS){
        position.position.z -= 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_W) == GLFW_PRESS){
        position.position.z += 0.01f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_Z) == GLFW_PRESS){
        position.rotation.y -= 5.f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_X) == GLFW_PRESS){
        position.rotation.y += 5.f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_1) == GLFW_PRESS){
        position.scale -= 0.1f;
    }
    if(glfwGetKey(window.window, GLFW_KEY_2) == GLFW_PRESS){
        position.scale += 0.1f;
    }

    //set vertex array for shaders to read data from
    buffer.bind();

    //this function draw elements according to shaders
    glDrawArrays(GL_TRIANGLES, 0, models.list.at(0).nVertices);
    //glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);
}

int main (){
    ModelList models;
    models.initDefaultList();

    Window window(700,1040);
    window.init();

    Shader shader("C:\\EngPathReq\\might_beeeeeeeeeeee\\vertex.vsh", "C:\\EngPathReq\\might_beeeeeeeeeeee\\fragment.fsh");
    shader.compileShaders();

    Buffer buffer(models.list.at(0), shader);

    TextureList texList;
    texList.loadTexture("C:/EngPathReq/might_beeeeeeeeeeee/box.png");
    texList.loadTexture("C:/EngPathReq/might_beeeeeeeeeeee/box1.png");

    //BufferList buffers...
    //BufferList should include model, shader for model, textureList for model

    Position position(shader);
    position.activate();

    Perspective perspective(shader, window);
    perspective.activate();

    Renderer renderer(models, window, shader, buffer, texList, position, perspective);
    renderer.setBackgroundColor(0.5f, 0.f, 0.f, 0.99f);
    renderer.render(drawFrame);
    return 0;
}
