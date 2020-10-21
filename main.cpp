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

        bool next = false;
        int lastPolyId = -1;

        while(!f.eof()){
            switchCase = -1;

            getline(f, line);
            removeBadSpaces(line);

            token = bite(" ", line, end);

            if(token == "v"){
                if(next){
                    lastPolyIds.push_back(lastPolyId);
                    next = false;
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
                next = true;
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

        if(next == true){
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
        string delimiter = "/";
        int numInd = calcNumArgsDividedBy("/", bite(" ", copyLine, end));

        vector<intvec3>* indexes = new vector<intvec3>;

        //cout << token << " ";
        while (true){
            if(numInd == 1){
                delimiter = " ";
                token = bite(delimiter, line, end);
                ind[0] = stoi(token);
                ind[1] = ind[2] = 1;//very important to set 1
            }
            if(numInd == 2){
                delimiter = "/";
                token = bite(delimiter, line, end);
                ind[0] = stoi(token);
                delimiter = " ";
                token = bite(delimiter, line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;
                ind[2] = 1;
            }
            if(numInd == 3){
                delimiter = "/";
                token = bite(delimiter, line, end);
                ind[0] = stoi(token);

                token = bite(delimiter, line, end);
                if(token != "")
                    ind[1] = stoi(token);
                else
                    ind[1] = 1;

                delimiter = " ";
                token = bite(delimiter, line, end);
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
    string name; //same as path

    Texture(){

    }
    Texture(string path){
        name = path;

        loadTexture();
    }
    Texture(Texture& t) = delete;
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
    void setPath(string path){
        this->name = path;
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
    void pushNew(string path){
        Texture* t = new Texture(path);
        list.push_back(t);
        unbindTextures();
    }
    void bindTextureByIndex(Shader* shader, GLint n, size_t textureIndex, string uniformName){
        shader->setUniform1i(uniformName, n);

        //use texture
        activeTexture(n);
        glBindTexture(GL_TEXTURE_2D, list.at(textureIndex)->textureId);
    }
//    void bindTextureByName(Shader* shader, GLenum GL_TEXTUREn, GLint n, string uniformName, string path){
//        shader->setUniform1i(uniformName, n);

//        //use texture
//        glActiveTexture(GL_TEXTUREn);
//        glBindTexture(GL_TEXTURE_2D, getByName(path)->textureId);
//    }
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
    void bindLayout(size_t layoutId, Shader* shader){
        TextureLayout* currLayout = &texLayouts.at(layoutId);
        for(size_t i = 0; i < currLayout->textureIndexes.size(); ++i){
            bindTextureByIndex(shader,
                               currLayout->ns.at(i),
                               currLayout->textureIndexes.at(i),
                               currLayout->uniformNames.at(i));
        }
    }
    size_t texLayoutsAmount(){
        return texLayouts.size();
    }
private:
    vector<TextureLayout> texLayouts;

    void activeTexture(GLint n){
        switch (n) {
        case 0:
            glActiveTexture(GL_TEXTURE0);
            break;
        case 1:
            glActiveTexture(GL_TEXTURE1);
            break;
        case 2:
            glActiveTexture(GL_TEXTURE2);
            break;
        case 3:
            glActiveTexture(GL_TEXTURE3);
            break;
        case 4:
            glActiveTexture(GL_TEXTURE4);
            break;
        case 5:
            glActiveTexture(GL_TEXTURE5);
            break;
        case 6:
            glActiveTexture(GL_TEXTURE6);
            break;
        case 7:
            glActiveTexture(GL_TEXTURE7);
            break;
        case 8:
            glActiveTexture(GL_TEXTURE8);
            break;
        case 9:
            glActiveTexture(GL_TEXTURE9);
            break;
        case 10:
            glActiveTexture(GL_TEXTURE10);
            break;
        case 11:
            glActiveTexture(GL_TEXTURE11);
            break;
        case 12:
            glActiveTexture(GL_TEXTURE12);
            break;
        case 13:
            glActiveTexture(GL_TEXTURE13);
            break;
        case 14:
            glActiveTexture(GL_TEXTURE14);
            break;
        case 15:
            glActiveTexture(GL_TEXTURE15);
            break;
        case 16:
            glActiveTexture(GL_TEXTURE16);
            break;
        case 17:
            glActiveTexture(GL_TEXTURE17);
            break;
        case 18:
            glActiveTexture(GL_TEXTURE18);
            break;
        case 19:
            glActiveTexture(GL_TEXTURE19);
            break;
        case 20:
            glActiveTexture(GL_TEXTURE20);
            break;
        case 21:
            glActiveTexture(GL_TEXTURE21);
            break;
        case 22:
            glActiveTexture(GL_TEXTURE22);
            break;
        case 23:
            glActiveTexture(GL_TEXTURE23);
            break;
        case 24:
            glActiveTexture(GL_TEXTURE24);
            break;
        case 25:
            glActiveTexture(GL_TEXTURE25);
            break;
        case 26:
            glActiveTexture(GL_TEXTURE26);
            break;
        case 27:
            glActiveTexture(GL_TEXTURE27);
            break;
        case 28:
            glActiveTexture(GL_TEXTURE28);
            break;
        case 29:
            glActiveTexture(GL_TEXTURE29);
            break;
        case 30:
            glActiveTexture(GL_TEXTURE30);
            break;
        case 31:
            glActiveTexture(GL_TEXTURE31);
            break;
        }
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

    void pushToShader(Shader* s){
        updateMatrices();
        s->setUniformMatrix4fv("ViewMatrix", glm::value_ptr(viewMatrix));
        s->setUniform3fv("cameraPos", glm::value_ptr(camera->position));
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

    void pushToShader(Shader* s){
        updateMatrices();
        s->setUniformMatrix4fv("ProjectionMatrix", glm::value_ptr(projectionMatrix));
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
    glm::vec3 location;
    glm::vec3 rotation;
    glm::vec3 scale;

    Position(){
        location = glm::vec3(0.f);
        rotation = glm::vec3 (0.f);
        scale = glm::vec3(1.f);
        modelMatrix = glm::mat4(1.f);
    }

    void pushToShader(Shader* s){
        updateMatrices();
        s->setUniformMatrix4fv("ModelMatrix", glm::value_ptr(modelMatrix));
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
        setEvent(w->window, 2, scale -= 0.1f);
        setEvent(w->window, 1, scale += 0.1f);
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
    void scaleTo(float x, float y, float z){
        scale.x = x;
        scale.y = y;
        scale.z = z;
    }
private:
    glm::mat4 modelMatrix;
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

class Material{
public:
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
    glm::vec3 ambientColor = glm::vec3(1.f);
    glm::vec3 diffuseColor = glm::vec3(1.f);
    glm::vec3 specularColor = glm::vec3(1.f);

    float specularHighlights = 1.f;
    float opticalDensity = 1.f;
    float dissolve = 1.f;

    //Texture* colorTexture;
};

class Object{
public:
    Window* window;
    TextureList* texList;
    Shader* shader;
    Buffer* buffer;
    Position* position;
    View* view;
    Perspective* perspective;
    LightSourceList* lightSources;
    Material* material;

    string name;

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

        //make positionm, perspective and view push like material push
        position->pushToShader(shader);
        perspective->pushToShader(shader);
        view->pushToShader(shader);
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
                if(textureI == texList->texLayoutsAmount()){
                    textureI = 0;
                }
                texList->bindLayout(textureI, shader);
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

class Objects : public List<Object>{

};

class Renderer{
public:
    Window* window;
    Perspective* perspective;
    View* view;
    MouseListener* mouse;
    CameraList* camList;

    float r, g, b, a;

    Renderer(int windowWidth, int windowHeight){
        window = new Window(windowWidth, windowHeight);

        perspective = new Perspective(*window);
        mouse = new MouseListener(*window);
        camList = new CameraList();
        camList->push(*(new Camera("default")));
        view = new View(*window, *mouse, *camList->at(0));
        objects = new Objects;
    }

    void addNewObject(TextureList& tl, Shader& s, Buffer& b, LightSourceList& lsl, Material& m){
        Object* o = new Object(*window, tl, s, b, *perspective, *view, lsl, m);
        objects->push(*o);
    }
    Object* getObjectByIndex(unsigned int index){
        return  objects->at(index);
    }
    Object* getObjectByName(string name){
        return objects->getByName(name);
    }
    void render(void (*frameFunction)(Renderer&)){
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
private:
    Objects* objects;
    bool doContinue = false;
};

void drawObject(Object& o){
    //TODO: #define bindTextureByIndex
    //o.texList->bindLayout(1, o.shader);

    o.lightSources->pushToShader(o.shader, 0, "lightPos0");

    //add all get set methods to reduce this one callback size
    //also do it with every class
    setEvent(o.window->window, F, o.lightSources->getByName("default")->lightPos = o.view->getCamera().position);
}
void drawFrame(Renderer& r){
    Object* currObj = r.getObjectByIndex(0);
    currObj->draw(drawObject);

    currObj = r.getObjectByIndex(1);
    currObj->draw(drawObject);

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
    bool test = false;
    if(test){
        MeshLoader meshLoader;

        MeshList meshList;
        //meshList.initDefaultList();
        meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\model_star.obj", "loaded"));
        //meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\model_test.obj", "loaded1"));
    }
    else{
        Renderer renderer(700, 1040);

        MeshLoader meshLoader;

        MeshList meshList;
        //meshList.initDefaultList();
        meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\model_dragon.obj", "loaded"));
        meshList.push(meshLoader.load("C:\\EngPathReq\\might_beeeeeeeeeeee\\model_phone.obj", "loaded1"));

        ShaderList shaders;
        shaders.pushNew("C:\\EngPathReq\\might_beeeeeeeeeeee\\vertex.vsh", "C:\\EngPathReq\\might_beeeeeeeeeeee\\fragment.fsh", "default");

        TextureList tex;
        tex.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/screen.jpg");//0
        //tex.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/box1.png");//1
        tex.addLayouts(1);
        tex.appendTextureToLayout(0, 0, 0, "texture0");
        //tex.appendTextureToLayout(0, GL_TEXTURE1, 1, 1, "specularTex");

        TextureList tex1;
        tex1.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/box.png");//0
        tex1.pushNew("C:/EngPathReq/might_beeeeeeeeeeee/ii.png");//1

        tex1.addLayouts(1);
        tex1.appendTextureToLayout(0, 0, 0, "texture0");
        tex1.appendTextureToLayout(0, 1, 1, "specularTex");

        //TODO: add events to light sources
        LightSourceList lightSources;
        lightSources.push(*new LightSource("default"));

        Buffer buffer(*meshList.at(0), *shaders.getByName("default"));
        Buffer buffer1(*meshList.at(1), *shaders.getByName("default"));

        Material mat;
        mat.setAmbientColor(0.01f,0.01f,0.01f);

        renderer.addNewObject(tex1, *shaders.getByName("default"), buffer, lightSources, mat);
        renderer.addNewObject(tex1, *shaders.getByName("default"), buffer1, lightSources, mat);
        renderer.getObjectByIndex(0)->scaleTo(0.01f, 0.01f, 0.01f);
        renderer.getObjectByIndex(1)->move(2.f,0,0);
        renderer.getObjectByIndex(1)->scaleTo(0.01f, 0.01f, 0.01f);

        renderer.setBackgroundColor(0.5f, 0.f, 0.f, 0.99f);
        renderer.render(drawFrame);
    }
    return 0;
}
