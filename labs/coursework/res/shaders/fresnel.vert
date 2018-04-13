#version 440
#define M_PI 3.1415926535897932384626433832795

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;


//scrolling offset
uniform vec3 offset;

//incoming position
layout (location = 0) in vec3 position;
//incoming normal
layout(location = 2) in vec3 normal;
//incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing position
layout(location = 0) out vec3 world_position;
// Outgoing normal
layout(location = 1) out vec3 transformed_normal;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord;
//debug colours
layout(location = 3) out flat vec4 debug_colours;

void main()
{
	float p = 1.5 * cos(position.x/20*2*M_PI + offset.x); 
  	gl_Position = MVP * vec4(position.x, p, position.z, 1);
	// Output texture coordinate to fragement shader
	world_position = vec3(M * vec4(position, 1.0f));
	transformed_normal = N * normal;
	tex_coord = tex_coord_in;

	debug_colours.r = sin(p*0.6);
  	debug_colours.g = -sin(p*0.6);
  	debug_colours.b = 0;
  	debug_colours.a = 1;
}