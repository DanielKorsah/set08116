#pragma once

#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace graphics_framework;
using namespace glm;
using namespace std;

vector<vec3> makeMesh(vector<vec3> &normals, int size);
void makeRow(vector<vec3> &verteces, vector<vec3> &normals, vec3 row_origin, int size);
void makeSplitQuad(vector<vec3> &verteces, vector<vec3> &normals, vec3 quad_origin);
void setNormal(vector<vec3> &normals, vec3 a, vec3 b);
void setTexCoords(vector<vec3> &positions, vector<vec2> &tex_coords, int size);
