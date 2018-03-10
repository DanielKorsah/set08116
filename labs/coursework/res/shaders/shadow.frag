#version 440

// This shader requires part_spot.frag, part_direction.frag, part_point.frag, shadow.frag

// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light
{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

/*
// Forward declarations of used functions
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);*/
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos) {
  // Get light screen space coordinate
  vec3 proj_coords = light_space_pos.xyz / light_space_pos.w;
  // Use this to calculate texture coordinates for shadow map
  vec2 shadow_tex_coords;
  // This is a bias calculation to convert to texture space
  shadow_tex_coords.x = 0.5 * proj_coords.x + 0.5;
  shadow_tex_coords.y = 0.5 * proj_coords.y + 0.5;
  // Check shadow coord is in range
  if (shadow_tex_coords.x < 0 || shadow_tex_coords.x > 1 || shadow_tex_coords.y < 0 || shadow_tex_coords.y > 1) {
    return 1.0;
  }
  float z = 0.5 * proj_coords.z + 0.5;
  // *********************************
  // Now sample the shadow map, return only first component (.x/.r)
  float depth = texture(shadow_map, shadow_tex_coords).x;
  // *********************************
  // Check if depth is in range.  Add a slight epsilon for precision
  if (depth == 0.0) {
    return 1.0;
  } else if (depth < z + 0.001) {
    return 0.5;
  } else {
    return 1.0;
  }
}

vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
 // *********************************
	// Calculate ambient component
    vec4 ambient = mat.diffuse_reflection * light.ambient_intensity;
	// Calculate diffuse component :  (diffuse reflection * light_colour) *  max(dot(normal, light direction), 0)
    vec4 diffuse = max(dot(normal, light.light_dir), 0.0) * (mat.diffuse_reflection * light.light_colour);
	// Calculate normalized half vector 
    vec3 half_vector = normalize(light.light_dir + view_dir);
	// Calculate specular component : (specular reflection * light_colour) * (max(dot(normal, half vector), 0))^mat.shininess
    float ks = pow(max(dot(normal, half_vector), 0.0), mat.shininess);
    vec4 specular = ks * (mat.specular_reflection * light.light_colour);
 // *********************************
	// Calculate colour to return
	vec4 colour = ((mat.emissive + ambient + diffuse) * tex_colour) + specular;
	colour.a = 1.0;
	// Return colour
	return colour;

}

vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// *********************************
	// Get distance between point light and vertex
	float d = distance(point.position, position);
	// Calculate attenuation factor : constant + (linear * d) + (quadratic * d * d)
	float attenuation = point.constant + point.linear * d + point.quadratic * pow(d, 2);
	// Calculate light colour : light_colour / attenuation
	vec4 light_col = (1/attenuation) * point.light_colour;
	//Set colour alpha to 1.0
	light_col.a = 1.0f;
	// Calculate light dir
	vec3 light_dir = normalize(point.position - position);
	// *********************************
	// Now use standard phong shading but using calculated light colour and direction
	// - note no ambient
	vec4 diffuse = (mat.diffuse_reflection * light_col) * max(dot(normal, light_dir), 0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_col) * pow(max(dot(normal, half_vector), 0), mat.shininess);
	vec4 primary = mat.emissive + diffuse;
	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
	return colour;
}

vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	vec3 light_dir = normalize(spot.position - position);
	float d = distance(spot.position, position);
	float att = spot.constant + spot.linear * d + spot.quadratic * d * d;
	float I = pow(max(dot(light_dir, -1.0 * spot.direction), 0.0), spot.power);
	vec4 light_colour = I / att * spot.light_colour;
	
	vec4 diffuse = (mat.diffuse_reflection * light_colour) * max(dot(normal, light_dir), 0.0);
	vec3 half_vector = normalize(light_dir + view_dir);
	vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0.0), mat.shininess);
	
	vec4 colour = ((mat.emissive + diffuse) * tex_colour) + specular;
	colour.a = 1.0;

	return colour;
}

// lights being used in the scene
uniform spot_light spot;
uniform directional_light light;
uniform point_light point;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Shadow map to sample from
uniform sampler2D shadow_map;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming light space position
layout(location = 3) in vec4 light_space_pos;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate shade factor
  float shade = calculate_shadow(shadow_map, light_space_pos);
  // Calculate view direction, normalize it
  vec3 view_dir = normalize(eye_pos - position);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);

  //calculate directional light
  colour = calculate_direction(light, mat, normal, view_dir, tex_colour);
  // Calculate spot light
  colour += calculate_spot(spot, mat, position, normal, view_dir, tex_colour);
  //calculate point light
  colour += calculate_point(point, mat, position, normal, view_dir, tex_colour);

  // Scale colour by shade
  colour *= shade;
  //Ensure alpha is 1.0
  colour.a = 1.0f;
  // *********************************
}