#include "meshGen.h"
#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace graphics_framework;
using namespace glm;
using namespace std;



	vector<vec3> makeMesh(vector<vec3> &normals, int size)
	{
		vector<vec3> verteces;

		vec3 start = vec3(-size / 2, 0.0f, -size / 2);
		vec3 row_origin = start;

		for (int i = 0; i < size; i++)
		{
			makeRow(normals, verteces, row_origin, size);
			row_origin.z += 1;
		}

		return verteces;
	}

	void makeRow(vector<vec3> &verteces, vector<vec3> &normals, vec3 row_origin, int size)
	{
		//make row
		for (int i = 0; i < size; i++)
		{
			makeSplitQuad(normals, verteces, row_origin);
			row_origin += vec3(1.0f, 0, 0);
		}
	}

	void makeSplitQuad(vector<vec3> &verteces, vector<vec3> &normals, vec3 quad_origin)
	{
		vec3 triangle_origin = quad_origin;

		//first triangle
		{
			verteces.push_back(triangle_origin);
			vec3 a = vec3(triangle_origin.x, triangle_origin.y, triangle_origin.z + 1);
			vec3 b = vec3(triangle_origin.x + 1, triangle_origin.y, triangle_origin.z + 1);

			verteces.push_back(a);
			verteces.push_back(b);

			setNormal(normals, a - triangle_origin , b -triangle_origin);

			
		}

		triangle_origin = vec3(triangle_origin.x + 1, triangle_origin.y, triangle_origin.z + 1);

		//second triangle
		{
			verteces.push_back(triangle_origin);

			vec3 a = vec3(triangle_origin.x, triangle_origin.y, triangle_origin.z - 1);
			vec3 b = vec3(triangle_origin.x - 1, triangle_origin.y, triangle_origin.z - 1);

			verteces.push_back(vec3(triangle_origin.x, triangle_origin.y, triangle_origin.z - 1));
			verteces.push_back(vec3(triangle_origin.x - 1, triangle_origin.y, triangle_origin.z - 1));

			setNormal(normals, triangle_origin - a, triangle_origin - b);
		}

	}

	void setNormal(vector<vec3> &normals, vec3 a, vec3 b)
	{
		vec3 norm = normalize(cross(a, b));

		normals.push_back(norm);
	}

	void setTexCoords(vector<vec3> &positions, vector<vec2> &tex_coords, int size)
	{
		for (vec3 p : positions)
		{
			vec2 coord;
			coord.x = p.x / size;
			coord.y = p.y / size;
			tex_coords.push_back(coord);
		}
	}

	