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

#define setEvent(GLuint_window, Button, Code)\
    if(glfwGetKey(GLuint_window, GLFW_KEY_##Button) == GLFW_PRESS){\
        Code;\
    }\

//TODO: divide some functions to smaller pieces
//TODO: add notes that camera and mesh classes needed to contain data, rename classes or smth else
struct Vertex{
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 texcoord;
    glm::vec3 normal;
};

struct intvec3{
    intvec3(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    int x;
    int y;
    int z;
};

template<class T>
class List{
public:
    //add check for repeating names in list
    vector<T*> list;

    size_t size(){
        return list.size();
    }
    void push(T& obj){
        list.push_back(&obj);
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
        return nullptr;
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

        init();
    }
    Window(Window& w) = delete;
    ~Window(){
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void setPolygonMode(GLenum face = GL_FRONT_AND_BACK, GLenum mode = GL_FILL){
        glPolygonMode(face, mode);
    }
    void setDrawOrder(bool isCounterclockwise){
        if(isCounterclockwise){
            glFrontFace(GL_CCW);
        }
        else{
            glFrontFace(GL_CW);
        }
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

class Mesh{
public:
    vector<Vertex>* vertices;
    vector<GLuint>* indices;
    vector<int> partEndVertexIds;

    unsigned int nVertices = 0;
    unsigned int nIndices = 0;

    string name;

    Mesh(string name = "noname"){
        this->name = name;

        vertices = new vector<Vertex>();
        this->indices = new vector<GLuint>();
    }

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
    void addPolyIndices(GLuint a, GLuint b, GLuint c){
        indices->push_back(a);
        indices->push_back(b);
        indices->push_back(c);
    }
};

class MeshLoader{
public:
    MeshLoader(){
        allPolyIndexes = new vector<vector<intvec3>>;
    }

    Mesh& load(string path, string meshName = "noname"){
        mesh = new Mesh(meshName);

        ifstream f;
        f.open(path);

        string line;

        int switchCase;
        bool end;

        bool nextPart = false;
        int lastPolyId = -1;

        while(!f.eof()){
            switchCase = -1;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "v"){
                if(nextPart){
                    lastPolyIds.push_back(lastPolyId);
                    nextPart = false;
                }
                //lastPolyId = -1;
                switchCase = 0;
            }
            if(token == "vn"){
                switchCase = 1;
            }
            if(token == "vt"){
                switchCase = 2;
            }
            if(token == "f"){
                ++lastPolyId;
                switchCase = 3;
                nextPart = true;
            }

            switch (switchCase) {
            case -1:
                break;
            case 0:
                poses.push_back(parsePos(line));
                break;
            case 1:
                norms.push_back(parseNorm(line));
                break;
            case 2:
                texes.push_back(parseNorm(line));
                break;
            case 3:
                allPolyIndexes->push_back(parseIndexes(line, calcNumArgsDividedBy(" ", line)));
                break;
            }
        }

        if(nextPart == true){
            lastPolyIds.push_back(lastPolyId);
        }
        buildMesh();

        return *mesh;
    }
private:
    Mesh* mesh;

    string token;

    vector<glm::vec3> poses;
    vector<glm::vec3> norms;
    vector<glm::vec2> texes;

    vector<intvec3> indexes;
    vector<vector<intvec3>>* allPolyIndexes;

    vector<int> lastPolyIds;

    void removeBadSpaces(string& line){
        size_t i = -1;
        while(true){
            ++i;
            if(i >= line.size() - 1 || line.size() < 2)
                break;
            if(line.at(i) == line.at(i+1) && line.at(i) == ' '){
                line.erase(line.begin()+i);
                --i;
            }
        }
        if(line.size() == 0)
            return;

        while(true){
            if(line.at(line.size()-1) == ' ')
                line.erase(line.begin() + line.size()-1);
            else
                break;
        }
        while(true){
            if(line.at(0) == ' ')
                line.erase(line.begin());
            else
                break;
        }
    }
    string& bite(const string& delimiter, string& line, bool& end){
        size_t pos = 0;
        string* token = new string;

        pos = line.find(delimiter);
        end = false;

        if(pos != string::npos){
            *token = line.substr(0, pos);
            line.erase(0, pos + delimiter.length());
        }
        else{
            end = true;
            token = &line;
        }
        return *token;
    }
    glm::vec3& parsePos(string& line){
        float pos[3];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 3)
                pos[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec3(pos[0], pos[1], pos[2]);
    }
    glm::vec3& parseNorm(string& line){
        float norm[3];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 3)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec3(norm[0], norm[1], norm[2]);
    }
    glm::vec2& parseTex(string& line){
        float norm[2];
        int i = -1;
        bool end;

        while (true){
            ++i;
            token = bite(" ", line, end);
            if(i < 2)
                norm[i] = stof(token);
            if(end)
                break;
        }
        return *new glm::vec2(norm[0], norm[1]);
    }
    vector<intvec3>& parseIndexes(string& line, int numArgsF){
        int ind[3];
        string copyLine = line;
        bool end;
        int numInd = calcNumArgsDividedBy("/", bite(" ", copyLine, end));

        vector<intvec3>* indexes = new vector<intvec3>;

        //cout << token << " ";
        while (true){
            if(numInd == 1){
                token = bite(" ", line, end);
                ind[0] = stoi(token);
                ind[1] = ind[2] = 1;//very important to set 1
            }
            if(numInd == 2){
                token = bite("/", line, end);
                ind[0] = stoi(token);
                token = bite(" ", line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;
                ind[2] = 1;
            }
            if(numInd == 3){
                token = bite("/", line, end);
                ind[0] = stoi(token);

                token = bite("/", line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;

                token = bite(" ", line, end);
                if(token != "")
                    ind[2] = stoi(token);
                else
                    ind[2] = 1;
            }

            --numArgsF;
            indexes->push_back(intvec3(ind[0], ind[1], ind[2]));

            if(numArgsF == 0){
                break;
            }
        }
        return *indexes;
    }
    int calcNumArgsDividedBy(string delimiter, string line){
        int numArgs = 0;
        string token;
        bool end;

        while(true){
            token = bite(delimiter, line, end);
            ++numArgs;
            if(end)
                break;
        }
        return numArgs;
    }
    void calcNormals(){
        for(size_t i = 0; i < mesh->vertices->size(); i += 3){
            glm::vec3 normal;
            normal = glm::normalize(glm::cross(mesh->vertices->at(i).position - mesh->vertices->at(i+1).position,
                                               mesh->vertices->at(i+1).position - mesh->vertices->at(i+2).position));
            if(glm::isnan(normal.x))
                normal = glm::vec3(0.f);

            mesh->vertices->at(i).normal = normal;
            mesh->vertices->at(i+1).normal = normal;
            mesh->vertices->at(i+2).normal = normal;
        }
    }
    void dividePolygonsToTriangles(){
        vector<intvec3> newPolyIndexes;
        vector<vector<intvec3>>* newAllPolyIndexes = new vector<vector<intvec3>>;
        int i;
        for(auto& currPolyIndexes : *allPolyIndexes){
            newPolyIndexes.clear();
            if(currPolyIndexes.size() > 3){
                i = 1;
                for(size_t j = 0; j < currPolyIndexes.size() - 2; j++){
                    newPolyIndexes.push_back(currPolyIndexes.at(0));
                    newPolyIndexes.push_back(currPolyIndexes.at(i));
                    newPolyIndexes.push_back(currPolyIndexes.at(i+1));
                    ++i;
                }
                newAllPolyIndexes->push_back(newPolyIndexes);
            }
            else{
                newAllPolyIndexes->push_back(currPolyIndexes);
            }
        }

        delete allPolyIndexes;
        allPolyIndexes = newAllPolyIndexes;
    }
    void buildMesh(){
        dividePolygonsToTriangles();

        int startFrom = 0;
        for(size_t i = 0; i < lastPolyIds.size(); ++i){
            if(mesh->partEndVertexIds.size() > 0)
                mesh->partEndVertexIds.push_back(mesh->partEndVertexIds.at(i-1));
            else
                mesh->partEndVertexIds.push_back(0);
            for(int j = startFrom; j < lastPolyIds.at(i)+1; ++j){
                    mesh->partEndVertexIds.at(i) += allPolyIndexes->at(j).size();
            }
            startFrom = lastPolyIds.at(i)+1;
        }

        for(auto& currPolyIndexes : *allPolyIndexes){
            for(auto& indexVec : currPolyIndexes){
                Vertex v;
                v.position = poses.at(indexVec.x - 1);

                if(texes.size() != 0)
                    v.texcoord = texes.at(indexVec.y - 1);
                else
                    v.texcoord = glm::vec3(0.f);

                if(norms.size() != 0)
                    v.normal = norms.at(indexVec.z - 1);

                v.color = glm::vec3(0.f);

                mesh->vertices->push_back(v);
                ++mesh->nVertices;
            }
        }

        mesh->nIndices = 0;

        if(norms.size() == 0){
            calcNormals();
        }

        clean();
    }
    void clean(){
        poses.clear();
        norms.clear();
        texes.clear();
        indexes.clear();
        allPolyIndexes->clear();
        lastPolyIds.clear();
    }
};

class MeshList : public List<Mesh>{

};

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
    void setVertexAttribPointer(string layoutName, int vecSize, size_t offset){
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
        list.push_back(new Shader(vertexPath, fragmentPath, name));
    }
};

//maybe add BufferList
class Buffer{
public:
    Buffer(Mesh& m, Shader& s){
        mesh = &m;
        shader= &s;

        genBuffers();
    }
    Buffer(Buffer& b) = delete;
    ~Buffer(){
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
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
    Mesh& getMesh(){
        return *mesh;
    }

private:
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    Mesh* mesh;
    Shader* shader;

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
    void setLayouts(){
        shader->setVertexAttribPointer("vertex_position", 3, offsetof(Vertex, position));
        shader->setVertexAttribPointer("vertex_color", 3, offsetof(Vertex, color));
        shader->setVertexAttribPointer("vertex_texcoord", 2, offsetof(Vertex, texcoord));
        shader->setVertexAttribPointer("vertex_normal", 3, offsetof(Vertex, normal));
    }
    void genBuffers(){
        initVAO();
        initVBO();
        initEBO();
        setLayouts();

        //current vao should be set in while window loop
        unbind();
    }
};

class Texture{
public:
    GLuint textureId;
    string name;
    string path; //same as path

    Texture(string name = "noname"){
        this->path = "";
        this->name = name;
        glGenTextures(1, &textureId);
    }
    Texture(string path, string name = "noname"){
        this->path = path;
        this->name = name;
        glGenTextures(1, &textureId);
    }
    Texture(Texture& t) = delete;
    ~Texture(){
        glDeleteTextures(1, &textureId);
    }
    void pushToShader(Shader* shader, GLint n, string uniformName){
        shader->setUniform1i(uniformName, n);

        glActiveTexture(GL_TEXTURE0 + n);
        bind();
    }
    void bind(){
        glBindTexture(GL_TEXTURE_2D, textureId);
    }
    void loadTexture(){
        //load image
        image_width = 0;
        image_height = 0;

        image = SOIL_load_image(path.data(), &image_width, &image_height, NULL, SOIL_LOAD_RGBA);

        //init and active texture
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

        //free space
        SOIL_free_image_data(image);
    }
private:
    int image_width;
    int image_height;

    unsigned char* image;

};

struct TextureLayout{
    void pushTexture(GLint n, size_t textureIndex, string uniformName){
        ns.push_back(n);
        textureIndexes.push_back(textureIndex);
        uniformNames.push_back(uniformName);
        ++layoutSize;
    }

    int layoutSize = 0;
    vector<GLint> ns;
    vector<size_t> textureIndexes;
    vector<string> uniformNames;
};

class TextureList : public List<Texture>{
public:
    void loadNew(string path, string name = "noname"){
        Texture* t = new Texture(path, name);
        t->loadTexture();
        list.push_back(t);
        unbindTextures();
    }
    void pushTextureToShaderByIndex(Shader* shader, GLint n, size_t textureIndex, string uniformName){
        at(textureIndex)->pushToShader(shader, n, uniformName);
    }
    void pushTextureToShaderByName(Shader* shader, GLint n, string textureName, string uniformName){
        getByName(textureName)->pushToShader(shader, n, uniformName);
    }
    void unbindTextures(){
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void appendTextureToLayout(size_t layoutId, GLint n, size_t textureIndex, string uniformName){
        texLayouts.at(layoutId).pushTexture(n, textureIndex, uniformName);
    }
    void addLayouts(int amount){
        for(int i = 0; i < amount; ++i){
            texLayouts.push_back(TextureLayout());
        }
    }
    void pushLayoutToShader(size_t layoutId, Shader* shader){
        TextureLayout* currLayout = &texLayouts.at(layoutId);
        for(size_t i = 0; i < currLayout->textureIndexes.size(); ++i){
            pushTextureToShaderByIndex(shader,
                               currLayout->ns.at(i),
                               currLayout->textureIndexes.at(i),
                               currLayout->uniformNames.at(i));
        }
    }
    size_t layoutsAmount(){
        return texLayouts.size();
    }
private:
    vector<TextureLayout> texLayouts;
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

    bool firstMouse;

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

};

class View{
public:
    View(Window& w, MouseListener& ml, Camera& c){
        window = &w;
        mouse = &ml;
        camera = &c;

        worldUp = glm::vec3(0.f, 1.f, 0.f);

        viewMatrix = glm::lookAt(camera->position, camera->position + camera->front, camera->up);
    }

    void pushToShader(Shader* s, string viewMatrixUniformName, string cameraPosUniformName){
        updateMatrices();
        s->setUniformMatrix4fv(viewMatrixUniformName, glm::value_ptr(viewMatrix));
        s->setUniform3fv(cameraPosUniformName, glm::value_ptr(camera->position));
    }
    void updateMatrices(){
        updateCameraPosition();
        //camera->position + camera->front is camera direction vector
        viewMatrix = glm::lookAt(camera->position, camera->position + camera->front, camera->up);
    }
    void setDefaultEvents(){
        setEvent(window->window, A, camera->position -= camera->right * camera->movementSpeed * mouse->dt);
        setEvent(window->window, D, camera->position += camera->right * camera->movementSpeed * mouse->dt);
        setEvent(window->window, Q, camera->position -= camera->up * camera->movementSpeed * mouse->dt);
        setEvent(window->window, E, camera->position += camera->up * camera->movementSpeed * mouse->dt);
        setEvent(window->window, S, camera->position -= camera->front * camera->movementSpeed * mouse->dt);
        setEvent(window->window, W, camera->position += camera->front * camera->movementSpeed * mouse->dt)
    }
    void setCamera(Camera& c){
        camera = &c;
    }
    Camera& getCamera(){
        return *camera;
    }
    const Window& getWindowPtr(){
        return  *window;
    }
private:
    Window* window;
    MouseListener* mouse;

    Camera* camera;

    glm::mat4 viewMatrix;
    glm::vec3 worldUp;

    void updateCameraPosition(){
        camera->pitch += static_cast<GLfloat>(mouse->mouseOffsetY) * camera->sensitivity * mouse->dt;
        camera->yaw += static_cast<GLfloat>(mouse->mouseOffsetX) * camera->sensitivity * mouse->dt;

        if(camera->pitch > 80.f)
            camera->pitch = 80.f;
        else if (camera->pitch < -80.f)
            camera->pitch = -80.f;

        if(camera->yaw > 360.f || camera->yaw < -360.f){
            camera->yaw = 0;
        }

        //hypotenuse = 1, so yaw and pitch projection calculates by cosA = x/h = x, sinA = y
        //the coefficient is relative cause cos * cos and other projection combinations gives number in range [-1;1]
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
    float fov = 90.f;
    float nearPlane = 0.001f;
    float farPlane = 1000.f;

    Perspective(Window& w){
        window = &w;

        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }

    void pushToShader(Shader* s, string uniformName){
        updateMatrices();
        s->setUniformMatrix4fv(uniformName, glm::value_ptr(projectionMatrix));
    }
    void updateMatrices(){
        glfwGetFramebufferSize(window->window, &window->fbWidth, &window->fbHeight);
        projectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(window->fbWidth) / window->fbHeight, nearPlane, farPlane);
    }
    const Window& getWindowPtr(){
        return  *window;
    }

    //TODO: fill with smth later
    void setDefaultEvents(){}
private:
    glm::mat4 projectionMatrix;

    Window* window;
};

class Position{
public:
    Position(){
        location = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        modelMatrix = glm::mat4(1.f);
    }

    void pushToShader(Shader* s, string uniformName){
        updateMatrices();
        s->setUniformMatrix4fv(uniformName, glm::value_ptr(modelMatrix));
    }
    void updateMatrices(){
        modelMatrix = glm::mat4(1.f);
        modelMatrix = glm::translate(modelMatrix, location);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
    }

    void setDefaultEvents(Window* w){
        setEvent(w->window, 2, scale -= 0.01f);
        setEvent(w->window, 1, scale += 0.01f);
        setEvent(w->window, Z, rotation.y -= 5.f);
        setEvent(w->window, X, rotation.y += 5.f);
    }
    void move(float x, float y, float z){
        location.x += x;
        location.y += y;
        location.z += z;
    }
    void moveTo(float x, float y, float z){
        location.x = x;
        location.y = y;
        location.z = z;
    }
    void rotate(float x, float y, float z){
        rotation.x += x;
        rotation.y += y;
        rotation.z += z;
    }
    void rotateTo(float x, float y, float z){
        rotation.x = x;
        rotation.y = y;
        rotation.z = z;
    }
    void scaleTo(float x, float y, float z){
        scale.x = x;
        scale.y = y;
        scale.z = z;
    }
private:
    glm::mat4 modelMatrix;

    glm::vec3 location;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class LightSource{
public:
    string name;
    glm::vec3 lightPos;

    LightSource(string name = "noname"){
        this->name = name;
        lightPos = glm::vec3(0.f, 0.f, 2.f);
    }

    void setPosition(float x, float y, float z){
        lightPos.x += x;
        lightPos.y += y;
        lightPos.z += z;
    }
};

class LightSourceList : public List<LightSource>{
public:
    void pushNew(string name = "noname"){
        push(*(new LightSource(name)));
    }
    void pushToShaderByIndex(Shader* s, int lightSourceNum, string uniformName){
        LightSource* currentSource = at(lightSourceNum);
        s->setUniform3fv(uniformName.data(), glm::value_ptr(currentSource->lightPos));
    }
    void pushToShaderByName(Shader* s, string name, string uniformName){
        LightSource* currentSource = getByName(name);
        s->setUniform3fv(uniformName.data(), glm::value_ptr(currentSource->lightPos));
    }
};

class Material{
public:
    glm::vec3 ambientColor = glm::vec3(1.f);
    glm::vec3 diffuseColor = glm::vec3(1.f);
    glm::vec3 specularColor = glm::vec3(1.f);

    float specularHighlights = 1.f;
    float opticalDensity = 1.f;
    float dissolve = 1.f;

    void pushToShader(Shader* s, string uniformName){
        s->setUniform3fv(uniformName + ".ambientColor", glm::value_ptr(ambientColor));
        s->setUniform3fv(uniformName + ".diffuseColor", glm::value_ptr(diffuseColor));
        s->setUniform3fv(uniformName + ".specularColor", glm::value_ptr(specularColor));

        s->setUniform1f(uniformName + ".specularHighlights", specularHighlights);
        s->setUniform1f(uniformName + ".opticalDensity", opticalDensity);
        s->setUniform1f(uniformName + ".dissolve", dissolve);
    }

    void setAmbientColor(float r, float g, float b){
        ambientColor = glm::vec3(r,g,b);
    }
    void setDiffuseColor(float r, float g, float b){
        diffuseColor = glm::vec3(r,g,b);
    }
    void setSpecularColor(float r, float g, float b){
        specularColor = glm::vec3(r,g,b);
    }
    void setSpecularHighlights(float specularHighlights){
        this->specularHighlights = specularHighlights;
    }

    void setOticalDensity(float density){
        opticalDensity = density;
    }
    void setDissolve(float dissolve){
        this->dissolve = dissolve;
    }
private:

    //Texture* colorTexture;
};

class SkyboxTexture{
public:
    GLuint textureId;
    string name;
    vector<string> facePaths;

    SkyboxTexture(vector<string> facePaths, string name = "noname"){
        this->facePaths = facePaths;
        this->name = name;

        loadSkybox();
    }
    SkyboxTexture(SkyboxTexture& st) = delete;
    ~SkyboxTexture(){
        glDeleteTextures(1, &textureId);
    }

    void pushToShader(Shader* shader, GLint n, string uniformName){
        shader->setUniform1i(uniformName, n);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    }
    void loadSkybox(){
        //load image
        image_width = 0;
        image_height = 0;

        //init and active texture
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

        for(size_t i = 0; i < facePaths.size(); i++){
            image = SOIL_load_image(facePaths.at(i).data(), &image_width, &image_height, NULL, SOIL_LOAD_RGB);
            if(image){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
                SOIL_free_image_data(image);
            }
            else{
                cout << "Texture loading failed" << endl;
            }
        }

        //setup texture
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
private:
    int image_width;
    int image_height;

    unsigned char* image;
};

class SkyboxObject{
public:
    SkyboxTexture* skyboxTexture;

    Window* window;
    Shader* shader;
    View* view;
    Perspective* perspective;
    Buffer* buffer;
    Position* position;

    string name;

    SkyboxObject(SkyboxTexture& st, Window& w, Shader& s, Buffer& b, Perspective& p, View& v){
        skyboxTexture = &st;
        window = &w;
        shader = &s;
        buffer = &b;
        perspective = &p;
        view = &v;

        position = new Position();

        name = skyboxTexture->name;
    }
    SkyboxObject(SkyboxObject& t) = delete;

    void draw(){
        window->setDrawOrder(false);
        shader->bind();

        position->pushToShader(shader, "ModelMatrix");
        perspective->pushToShader(shader, "ProjectionMatrix");
        view->pushToShader(shader, "ViewMatrix", "cameraPos");
        skyboxTexture->pushToShader(shader, 0, "skybox");

        position->setDefaultEvents(window);//remove this later

        buffer->bind();

        if (buffer->getMesh().nIndices != 0)
            glDrawElements(GL_TRIANGLES, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else
            glDrawArrays(GL_TRIANGLES, 0, buffer->getMesh().nVertices);

        buffer->unbind();
        shader->unbind();
        window->setDrawOrder(true);
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
    void rotateTo(float x, float y, float z){
        position->rotateTo(x,y,z);
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
};

class SkyboxList : public List<SkyboxObject>{

};

class Object{
public:
    Window* window;
    TextureList* texList;
    Shader* shader;//shader same as in buffer, delete this
    Buffer* buffer;
    Position* position;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;
    Material* material;

    string name;

    //make arguments optional
    Object(Window& w, TextureList& t,
           Shader& s, Buffer& b,
           Perspective& p, View& v,
           LightSourceList& lsl, Material& m,
           string name = "noname")
    {
        if (w.window != p.getWindowPtr().window || p.getWindowPtr().window != v.getWindowPtr().window){
            cout << "Perspective and view passed in \"Object\" constructor must have pointers to the same \"Window\" object\n";
            cout << "Object not created\n";
            return;
        }

        window = &w;
        texList = &t;
        shader = &s;
        buffer = &b;
        perspective = &p;
        view = &v;
        lightSources = &lsl;
        material = &m;
        this->name = name;

        position = new Position();
    }

    void draw(void (*drawObjectFunction)(Object&)){
        shader->bind();

        position->pushToShader(shader, "ModelMatrix");
        perspective->pushToShader(shader, "ProjectionMatrix");
        view->pushToShader(shader, "ViewMatrix", "cameraPos");
        material->pushToShader(shader, "material");

        position->setDefaultEvents(window);

        buffer->bind();

        drawObjectFunction(*this);

        if (buffer->getMesh().nIndices != 0)
            glDrawElements(GL_TRIANGLES, buffer->getMesh().nIndices, GL_UNSIGNED_INT, (void*)0);
        else{
            Mesh* currMesh = &buffer->getMesh();
            //cout << "Draw mesh: " << currMesh->name << endl;
            int startFrom = 0;
            bool first = true;
            size_t textureI = 0;
            for(size_t i = 0; i < currMesh->partEndVertexIds.size(); ++i){
                if(textureI == texList->layoutsAmount()){
                    textureI = 0;
                }
                texList->pushLayoutToShader(textureI, shader);
                ++textureI;

                if(first){
                    //cout << "Draw " << i << " part." << endl;
                    first = false;
                    glDrawArrays(GL_TRIANGLES, startFrom, currMesh->partEndVertexIds.at(i));
                }
                else{
                    //cout << "Draw " << i << " part." << endl;
                    glDrawArrays(GL_TRIANGLES, startFrom, currMesh->partEndVertexIds.at(i) - currMesh->partEndVertexIds.at(i-1));
                    startFrom = currMesh->partEndVertexIds.at(i);
                }
            }
        }

        buffer->unbind();
        shader->unbind();
        texList->unbindTextures();
    }

    //remove and make object name same as mesh name
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
    void rotateTo(float x, float y, float z){
        position->rotateTo(x,y,z);
    }
    void scaleTo(float x, float y, float z){
        position->scaleTo(x,y,z);
    }
};

class Objects : public List<Object>{

};

class Framebuffer{
public:
    GLuint FBO;
    GLuint RBO;
    TextureList* textureColorBuffers;

    int width;
    int height;

    Framebuffer(int width, int height){
        this->width = width;
        this->height = height;
        this->textureColorBuffers = new TextureList;
        textureColorBuffers->addLayouts(1);

        glGenFramebuffers(1, &FBO);
        glGenRenderbuffers(1, &RBO);
    }

    void genTextureColorBuffers(size_t amount){
        for(size_t i = 0; i < amount; i++){
            Texture* t = new Texture;
            textureColorBuffers->push(*t);
            textureColorBuffers->appendTextureToLayout(0, i, i, "textureColorBuffer"+to_string(i));
        }
    }
    void genColorAttachmentFramebuffer(GLuint attachmentNum){
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        textureColorBuffers->at(attachmentNum)->bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + attachmentNum,
                               GL_TEXTURE_2D,
                               textureColorBuffers->at(attachmentNum)->textureId,
                               0);
    }
    void attachRenderBuffer(){
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Framebuffer is not complete!" << endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void bind(){
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }
    void unbind(){
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
};

class FramebufferList : public List<Framebuffer>{

};

class Renderer{
public:
    Window* window;
    Perspective* perspective;
    View* view;
    MouseListener* mouse;
    CameraList* camList;
    SkyboxList* skyboxes;
    FramebufferList* framebuffers;

    float r, g, b, a;

    Renderer(int windowWidth, int windowHeight){
        window = new Window(windowWidth, windowHeight);

        perspective = new Perspective(*window);
        mouse = new MouseListener(*window);
        camList = new CameraList();
        camList->push(*(new Camera("default")));
        view = new View(*window, *mouse, *camList->at(0));
        objects = new Objects;
        skyboxes = new SkyboxList;
        framebuffers = new FramebufferList;
    }

    //make arguments optional
    void addNewObject(TextureList& tl, Shader& s, Buffer& b, LightSourceList& lsl, Material& m){
        Object* o = new Object(*window, tl, s, b, *perspective, *view, lsl, m);
        objects->push(*o);
    }
    void addNewSkybox(Shader& s, vector<string> facePaths, Buffer& b, string name = "noname"){
        SkyboxTexture* st = new SkyboxTexture(facePaths, name);
        SkyboxObject* so = new SkyboxObject(*st, *window, s, b, *perspective, *view);
        skyboxes->push(*so);
    }
    void addFramebuffer(Framebuffer& fb){
        framebuffers->push(fb);
    }
    Object* getObjectByIndex(unsigned int index){
        return objects->at(index);
    }
    Object* getObjectByName(string name){
        return objects->getByName(name);
    }
    SkyboxObject* getSkyboxObjectByIndex(unsigned int index){
        return skyboxes->at(index);
    }
    SkyboxObject* getSkyboxObjectByName(string name){
        return skyboxes->getByName(name);
    }
    void render(void (*frameFunction)(Renderer&)){
        doContinue = true;
        while (!glfwWindowShouldClose(window->window))
        {
            glfwPollEvents();

            //get enter press event to fast close
            if(glfwGetKey(window->window, GLFW_KEY_ENTER) == GLFW_PRESS){
                doContinue = false;
            }

            mouse->update();
            perspective->setDefaultEvents();
            view->setDefaultEvents();

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
    void bindBackgroundColor(){
        glClearColor(r, g, b, a);
    }
    void clearBuffers(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }
private:
    Objects* objects;
    bool doContinue = false;
};

void drawObject(Object& o){
    o.lightSources->pushToShaderByIndex(o.shader, 0, "lightPos0");

    //add all get set methods to reduce this one callback size
    //also do it with every class
    setEvent(o.window->window, F, o.lightSources->getByName("default")->lightPos = o.view->getCamera().position);
}
void drawFrame(Renderer& r){
    Object* currObj;

    r.framebuffers->at(0)->bind();

    r.bindBackgroundColor();
    r.clearBuffers();

    currObj = r.getObjectByIndex(0);
    currObj->draw(drawObject);

    currObj = r.getObjectByIndex(1);
    currObj->draw(drawObject);

    r.framebuffers->at(0)->unbind();


    r.bindBackgroundColor();
    r.clearBuffers();

    currObj = r.getObjectByIndex(2);
    currObj->draw(drawObject);

    r.skyboxes->at(0)->draw();

    static float defaultSpeed = r.view->getCamera().movementSpeed;

    bool isShiftPressed = false;
    setEvent(r.window->window, LEFT_SHIFT, isShiftPressed = true);
    if(isShiftPressed){
        r.view->getCamera().movementSpeed = defaultSpeed * 2;
    }
    bool isCtrlPressed = false;
    setEvent(r.window->window, LEFT_CONTROL, isCtrlPressed = true);
    if(isCtrlPressed){
        r.view->getCamera().movementSpeed = defaultSpeed / 3;
    }
    if(!isShiftPressed && !isCtrlPressed){
        r.view->getCamera().movementSpeed = defaultSpeed;
    }
}

int main (){
    Renderer renderer(700, 1040);
    MeshLoader meshLoader;
    MeshList meshList;
    ShaderList shaders;
    TextureList tex;
    LightSourceList lightSources;

    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\dragon.obj", "loaded"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\skybox.obj", "skybox"));
    meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\models\\quad.obj", "quad"));

    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\fragment.fsh", "default");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\skybox_fragment.fsh", "skybox");
    shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_vertex.vsh",
                    "C:\\EngPathReq\\might_beeeeeeeeeeee\\shaders\\screen_fragment.fsh", "screen");

    tex.loadNew("C:/EngPathReq/might_beeeeeeeeeeee/pictures/box.png");//0
    tex.loadNew("C:/EngPathReq/might_beeeeeeeeeeee/pictures/box1.png");//1

    tex.addLayouts(1);
    tex.appendTextureToLayout(0, 0, 0, "texture0");
    tex.appendTextureToLayout(0, 1, 1, "specularTex");

    //TODO: do something with lightsources
    lightSources.push(*new LightSource("default"));

    Buffer buffer(*meshList.getByName("loaded"), *shaders.getByName("default"));
    Buffer skyboxCube(*meshList.getByName("skybox"), *shaders.getByName("skybox"));
    Buffer quad(*meshList.getByName("quad"), *shaders.getByName("screen"));

    Material mat;
    mat.setAmbientColor(0.01f,0.01f,0.01f);
    mat.setSpecularHighlights(300);

    Framebuffer framebuffer(800,600);
    framebuffer.genTextureColorBuffers(1);
    framebuffer.genColorAttachmentFramebuffer(0);
    framebuffer.attachRenderBuffer();
    renderer.addFramebuffer(framebuffer);

    renderer.addNewObject(tex, *shaders.getByName("default"), buffer, lightSources, mat);
    renderer.addNewObject(tex, *shaders.getByName("default"), buffer, lightSources, mat);
    renderer.addNewObject(*framebuffer.textureColorBuffers, *shaders.getByName("screen"), quad, lightSources, mat);
    renderer.getObjectByIndex(0)->scaleTo(0.01f, 0.01f, 0.01f);
    renderer.getObjectByIndex(1)->move(2.f,0,0);
    renderer.getObjectByIndex(1)->scaleTo(0.01f, 0.01f, 0.01f);

    vector<string> skyboxSides;
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\right.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\left.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\top.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\bottom.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\front.jpg");
    skyboxSides.push_back("C:\\EngPathReq\\might_beeeeeeeeeeee\\skybox\\back.jpg");

    renderer.addNewSkybox(*shaders.getByName("skybox"), skyboxSides, skyboxCube);
    renderer.getSkyboxObjectByIndex(0)->scaleTo(5,5,5);

    renderer.setBackgroundColor(0.5f, 0.f, 0.f, 0.99f);
    renderer.render(drawFrame);
    return 0;
}
