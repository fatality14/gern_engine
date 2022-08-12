#include <render/renderer.h>
#include <render/sceneloader.h>
#include <chrono>
#include <filesystem>

int main (int argc, char *argv[]){
    int window_width = 1366;
    int window_height = 768;

    Renderer renderer(window_width, window_height);
    SceneLoader sl;

//    auto start = std::chrono::steady_clock::now();
    try{
        if(argc == 2){
            const string path = argv[1];
            if(path.size() != 0){
                sl.load(path, *renderer.model);
            }
        }
        else{
            throw string("You must specify scene file path as a first argument.");
        }
    }
    catch(string e){
        cout << e << endl;
        return -1;
    }
    catch(...){
        cout << "Unresolved error caught\n";
        return -1;
    }

//    auto end = std::chrono::steady_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end-start;
//    std::cout << "elapsed time: " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed_seconds).count() << "s\n";

    renderer.renderLoop();

    return 0;
}
