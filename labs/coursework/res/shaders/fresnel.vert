#version 440
#define M_PI 3.1415926535897932384626433832795

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;

//scrolling offset
uniform vec3 offset;



//incoming position
layout (location = 0) in vec3 position;
//incoming normal
layout(location = 2) in vec3[] normal;
//incoming texture coordinate
layout (location = 10) in vec2 tex_coord_in;

// Outgoing position
//incoming normal
layout(location = 2) out vec3 normal_out;
// Outgoing texture coordinate
layout(location = 3) out vec2 tex_coord_out;
//debug colours
layout(location = 4) out flat vec4 debug_colours;
//texcooord for reflection
layout (location = 5) out vec3 reflect_tex_coord_out;


void main()
{
	float p = /*amplitude*/0.3 * (cos(position.x/20*(2*M_PI) * (2 /*frequency*/) + offset.x));
	p +=  0.3 * (cos(position.z/20*2*M_PI * 2 + offset.x));
	p *= 10000000;
	round(p);
	p /= 10000000;
  	gl_Position = MVP * vec4(position.x, p, position.z, 1);

	// Output texture coordinate to fragement shader
	tex_coord_out = tex_coord_in;


	debug_colours.r = sin(p*0.8)+1;
  	debug_colours.g = -sin(p*0.8)+1;
  	debug_colours.b = 0;
  	debug_colours.a = 1;
}