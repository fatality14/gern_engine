#include <common.h>
#include <renderer.h>
#include <meshloader.h>
#include <skeletonobject.h>
#include <skeletonloader.h>
#include <shaderfield.h>
#include <instancedobject.h>
#include <sceneloader.h>
#include <materialloader.h>

int main (){
    int window_width = 1366;
    int window_height = 768;

    Renderer renderer(window_width, window_height);
    SceneLoader sl;

    SceneLoaderD sld;
    sld.load("C:\\Users\\1234\\Documents\\qtprojs\\ogl\\resources\\scenes\\test.txt", *renderer.model);

    sl.load("C:\\Users\\1234\\Documents\\qtprojs\\ogl\\resources\\scenes\\test.txt", renderer);

    renderer.renderLoop();

    return 0;
}
