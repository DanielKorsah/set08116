#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 vertex_position;
// Incoming normal
layout(location = 1) in vec3 transformed_normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Get distance between point light and vertex
  float dist = distance(point.position, vertex_position);
  // Calculate attenuation factor
  float attenuation = point.constant + point.linear * dist + point.quadratic* pow(dist, 2);
  // Calculate light colour
  vec4 light_col =  (1/(attenuation)) * point.light_colour;

  // Calculate light dir
  vec3 light_dir = normalize(point.position- vertex_position);
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
  vec4 diffuse = max(dot(transformed_normal, light_dir), 0.0f) * (mat.diffuse_reflection * light_col);
  vec3 view_dir = normalize(eye_pos - vertex_position);
  vec3 half_v = normalize(light_dir + view_dir);
  vec4 specular = pow(max(dot(transformed_normal, half_v), 0.0f), mat.shininess) * (mat.specular_reflection * light_col);
  vec4 tex_colour = texture(tex, tex_coord);
  vec4 primary = mat.emissive + diffuse;
  colour = primary * tex_colour + specular;
  colour.a = 1.0f;
}