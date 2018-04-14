#version 440

// A directional light structure
struct directional_light {
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};


// Point light information
struct point_light {
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
};

//Spot light structure
struct spot_light {
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};

// A material structure
struct material {
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};

// Directional light for the scene
uniform directional_light sun;

// Point light for lighting sun texture
uniform point_light point;

// Spot light being used 
uniform spot_light spot;

// Material of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

//uniform sampler2d transparent_tex;
uniform sampler2D refraction_tex;


// Incoming position
layout(location = 0) in vec3 world_position;

// Incoming normal
layout(location = 1) in vec3 transformed_normal;

// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

//debug colours
layout(location = 3)in flat vec4 debug_colours;

// Outgoing colour
layout(location = 0) out vec4 colour;


vec4 DirCalc()
{
// Set out colour to sampled texture colour
	vec4 ambient = mat.diffuse_reflection * sun.ambient_intensity;
	// Calculate diffuse component
	vec4 diffuse = max(dot(transformed_normal, -sun.light_dir), 0.0f) * (mat.diffuse_reflection * sun.light_colour);
	// Calculate view direction
	vec3 view_dir = normalize(eye_pos - world_position);
	// Calculate half vector
	vec3 half_v = normalize(-sun.light_dir + view_dir);
	// Calculate specular component
	vec4 specular =  pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * sun.light_colour);
	// Sample texture
	vec4 tex_colour = texture(tex, tex_coord);
	// Calculate primary colour component
	vec4 primary = mat.emissive + ambient + diffuse;

	// Calculate final colour
	vec4 dir = primary * tex_colour + specular;
	return dir;
}		

vec4 PointCalc()
{
	// Get distance between point light and vertex
	float d = distance(point.position, world_position);
	// Calculate attenuation factor
	float att = point.constant + point.linear * d + point.quadratic* pow(d, 2);
	// Calculate light colour
	vec4 light_col =  (1/(att)) * point.light_colour;

	// Calculate light dir
	vec3 light_dir = normalize(point.position- world_position);
	// Now use standard phong shading but using calculated light colour and direction
	// - note no ambient
	vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_col);
	vec3 view_dir = normalize(eye_pos - world_position);
	vec3 half_v = normalize(light_dir + view_dir);
	vec4 specular = pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * light_col);
	vec4 tex_colour = texture(tex, tex_coord);
	vec4 primary = mat.emissive + diffuse;

	vec4 point = primary * tex_colour + specular;
	return point;
}

vec4 SpotCalc()
{
	// Calculate direction to the light
  vec3 light_dir = normalize(spot.position - world_position);
  // Calculate distance to light
  float d = distance(spot.position, world_position);
  // Calculate attenuation value
  float att = 1.0f / (spot.constant + spot.linear * d + spot.quadratic * (d * d));
  // Calculate spot light intensity
  float light_int = pow(max(dot(-spot.direction, light_dir), 0.0), spot.power);
  // Calculate light colour
  vec4 light_colour_obj = light_int * att * spot.light_colour;
  // Calculate view direction
  vec3 view_dir = normalize(eye_pos - world_position);

  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0) * (mat.diffuse_reflection * light_colour_obj);
  // Calculate half vector
  vec3 half_vector = normalize(light_dir + view_dir);
  // Calculate specular component
  float ks = pow(max(dot(transformed_normal, half_vector), 0.0), mat.shininess);
  vec4 specular = ks * (mat.specular_reflection * light_colour_obj);
  // Sample texture
  vec4 tex_colour = texture(tex, tex_coord);
  // Calculate primary colour component
  vec4 primary = mat.emissive + diffuse;
  // Calculate final colour - remember alpha
  vec4 spot = primary * tex_colour + specular;

  return spot;
}

vec4 Fresnel(vec4 lit_colour)
{
    vec3 view_dir = normalize(eye_pos - world_position);
    float opacity = 0;
	vec4 transparent = vec4(1, 1, 1, 0);
    //dot of view and norm = cos(angle) --- where 90 degree angle to norm = cos(90) = zero opacity
	vec4 tex_colour = texture(refraction_tex, tex_coord);
	tex_colour.a = 1;
    opacity = dot(transformed_normal, view_dir);
	
    
	lit_colour += tex_colour;
	vec4 mix_colour = mix(lit_colour, transparent, opacity);
    
    return mix_colour;

    //visual debugging
	vec4 db;
    if(opacity< 0){
        db = vec4(0,1,0,1);
	}
    else if(opacity == 0){
        db = vec4(1,0,0,1);
	}
	else if(opacity > 0){
		db = vec4(1,0,1,1);
	}
    else
		db = vec4(1, 1, 1, 1);


	//return db;
}

													
void main() {
    
	vec4 all_lights = vec4(0.0);
	all_lights+= DirCalc();
	//all_lights+= PointCalc();
	//all_lights+= SpotCalc();
	//colour = debug_colours;
	colour = Fresnel(debug_colours);
	//colour = all_lights;
    
  }