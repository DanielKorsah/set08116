#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace graphics_framework;
using namespace glm;
using namespace std;

vector<vec3> makeMesh(int size);
void makeRow(vector<vec3> &verteces, vec3 row_origin, int size);
void makeSplitQuad(vector<vec3> &verteces, vec3 quad_origin);
