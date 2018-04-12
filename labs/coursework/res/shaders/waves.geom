#version 440

#define M_PI 3.1415926535897932384626433832795


layout(points) in;
layout(points, max_vertices = 1) out;
//Not important to this shader
/////////////
// Incoming primary colour
layout(location = 0) in vec4 primary[];
// Incoming secondary colour
layout(location = 1) in vec4 secondary[];
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord_out[];

// Outgoing primary colour
layout(location = 0) out vec4 primary_out;
// Outgoing secondary colour
layout(location = 1) out vec4 secondary_out;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord_geom;
/////////////////

void main()
{
    vec3 P0 = gl_in[0].gl_Position.xyz;
    //gl_in.gl_Position.xyz;
    gl_Position.y = (cos(gl_in[0].gl_Position.z/1080)*2*M_PI)/2;
    EmitVertex();
    EndPrimitive();
}