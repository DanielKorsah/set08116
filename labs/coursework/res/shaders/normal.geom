#version 440
#define M_PI 3.1415926535897932384626433832795


layout ( triangles ) in;
layout ( triangle_strip, max_vertices = 3 ) out;


// The normal matrix in
uniform mat3 N;
uniform mat4 M;

//incoming normal
layout(location = 2) in vec3[] normal_out;
// texture coordinate from frag
layout(location = 3) in vec2[] tex_coord;
//debug colours
layout(location = 4) in flat vec4[] debug_colours;




layout(location = 0) out vec3 world_position;
// Outgoing normal
layout(location = 1) out vec3 transformed_normal;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord_g;
//debug colours
layout(location = 3) out flat vec4 debug_colours_g;


vec3 calculateNormal()
{
	vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 normal = cross(a, b);	
	return normalize(normal);
}


void main(void)
{
    transformed_normal = calculateNormal();
    for(int i = 0; i<3; i++)
    {
        vec3 world_position = vec3(gl_in[i].gl_Position);
  	    gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }  
    EndPrimitive();
}