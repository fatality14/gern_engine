# Gern engine
## OpenGL rendering engine. <br> A wrapper around the standart OpenGL API designed to simplify custom C++ code embedding.

### Project structure
Root folder:
* `engine` folder - contains a source code of the program
* `libs` folder - contains third party libraries
* `resources` folder - contains user resource files like .obj or .png
* `shaders` folder - contains .glsl shader files
* `thirdparty` folder - contains third party source files
* `SOIL2GUIDE` file - recomendations how to compile soil with cmake
* `TODO` file - list of tasks
* `main.cpp` - main executable file with minimal sample program

`engine/render` folder contains classes to specify the user's render logic.

It is written in MVC pattern, where:
* `framecontroller.h` - has a controller class with sample frame draw logic
* `framemodel.h` - has a model class with interface to get stored data and e.g. draw it in the specified order using controller class
* view is represented with `renderer.h` and `irenderer.h` files which in its turn performs system calls to GLFW and OpenGL
* you could add your own logic in scene loading classes (`sceneloader.h` file) with the common command interface
* `window.h` - contains a wrapper to GLFW and OpenGL properties setup interface
* `mouselistener.h` - is used to detect mouse position

Other source code folders contains OOP wrapper of common OpenGL capabilities like framebuffers, cubemaps, shaders, meshes and most of related concepts eventually used in `*Object` classes. E.g. you could add yet another shader uniform if call `shaderFields.push()` method of samecalled `*Object` class field.

```
TODO: add more explanations here
```

 ### Dependencies
* GLEW32
* GLFW3
* SOIL2
* opengl
* glm

### Compiling
Current build compilation was performed using:
* MinGW 7.3 x64
* CMake 3.5
* ccache 4.6 (optional, *might be enabled in CMakeLists*)

Higher versions might work as well.

It is recommended to use QtCreator to ease a process though.

#### Windows users:
Simply start `cmake` in the project folder or drag-and-drop `CMakeLists.txt` in QtCreator and hit run.

Libs folder contains all suitable dll's to build app properly.

#### Linux user:
Compile specified project dependencies and replace corresponding files in `libs` folder.

Then simply start `cmake` in the project folder or drag-and-drop `CMakeLists.txt` in QtCreator and hit run.

```
TODO: add more explanations here
```

### Options
Startup:
* `main.cpp` file should have user specified scene file path in `sl.load()` line
* standart error emmited by programm has a `string` type, so `sl.load()` must be wrapped in `try...catch` block using this type
* `renderer.renderLoop()` start the OpenGL draw loop if there's no errors in data structures passed

Scene files:

```
TODO: add more explanations here
```

Animation files:

```
TODO: add more explanations here
```

Skeleton files:

```
TODO: add more explanations here
```

Material files:

```
TODO: add more explanations here
```

Shader files:

```
TODO: add more explanations here
```

### Bugfix, feature requests and false hopes
Autor have no enough time to fix all bugs or provide guaranteed linux compatibility.

Autor know about most bugs and defect (such as user defined exceptions in place of string exceptions)
in the program but he's too busy to spend lotta time fixing them.

If you stumbled upon this project and this .md line by accident, autor is sorry for his laziness.
