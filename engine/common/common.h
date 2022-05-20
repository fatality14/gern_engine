#pragma once

#include <iostream>
#include <fstream>

#include <common/gl.h>
#include <common/glm.h>

#include <SOIL2.h>
#include <vector>

using namespace std;

float someCounter = 0;


class ICommon{public :virtual ~ICommon(){}};

//TODO: add notes that camera and mesh classes are just data containers, rename classes or smth else
//TODO: make methods to access the objects of built-in classes
