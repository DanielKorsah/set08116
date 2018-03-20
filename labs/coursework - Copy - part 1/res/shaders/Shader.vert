#version 440

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;

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

void main()
{
	// Calculate screen position of vertex
	gl_Position = MVP * vec4(position, 1.0);
	// Output texture coordinate to fragement shader
	world_position = vec3(M * vec4(position, 1.0f));
	transformed_normal = N * normal;
	tex_coord = tex_coord_in;
}