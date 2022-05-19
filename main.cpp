#include <render/renderer.h>
#include <render/sceneloader.h>
#include <chrono>

int main (){
    int window_width = 1366;
    int window_height = 768;

    Renderer renderer(window_width, window_height);
//    SceneLoader sl;

    SceneLoader sl;

//    auto start = std::chrono::steady_clock::now();
    try{
        sl.load("C:\\Users\\1234\\Documents\\qtprojs\\ogl\\resources\\scenes\\test.txt", *renderer.model);
    }
    catch(string e){
        int a;
        cout << e << endl;
        return -1;
    }

//    auto end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end-start;
//    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed_seconds).count() << "s\n";

    renderer.renderLoop();

    return 0;
}
