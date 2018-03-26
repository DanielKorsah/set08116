#include "meshGen.h"
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace graphics_framework;
using namespace glm;
using namespace std;

vector<vec3> makeMesh(int size)
{
	vector<vec3> verteces;

	vec3 start = vec3(-size / 2, 0.0f, -size / 2);
	vec3 row_origin = start;

	for (int i = 0; i < size; i++)
	{
		makeRow(verteces, row_origin, size);
		row_origin.z += 1;
	}

	return verteces;
}


void makeRow(vector<vec3> &verteces, vec3 row_origin, int size)
{
	//make row
	for (int i = 0; i < size; i++)
	{
		makeSplitQuad(verteces, row_origin);
		row_origin += vec3(1.0f, 0, 1.0f);
	}
}

void makeSplitQuad(vector<vec3> &verteces, vec3 quad_origin)
{
	vec3 triangle_origin = quad_origin;

	//first triangle
	{
		verteces.push_back(triangle_origin);
		verteces.push_back(vec3(triangle_origin.x, triangle_origin.y, triangle_origin.z + 1));
		verteces.push_back(vec3(triangle_origin.x + 1, triangle_origin.y, triangle_origin.z + 1));
	}

	triangle_origin = vec3(triangle_origin.x + 1, triangle_origin.y, triangle_origin.z + 1);

	//second triangle
	{
		verteces.push_back(triangle_origin);
		verteces.push_back(vec3(triangle_origin.x, triangle_origin.y, triangle_origin.z - 1));
		verteces.push_back(vec3(triangle_origin.x - 1, triangle_origin.y, triangle_origin.z - 1));
	}

}