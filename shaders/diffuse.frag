#version 330 core



uniform vec3 ambience;

in vec3 normal;		//mesh normal vector
in vec3 FragPos;	//position vector for the vertex
in vec2 texCoords;	//texture coordinate for the vertex

out vec4 FragColor;

uniform vec3 cameraPos;

vec3 gamma_correction(vec3 color, float gamma){
return vec3(pow(color.x, 1.0/gamma), pow(color.y, 1.0/gamma),pow(color.z, 1.0/gamma));
}

struct pointLight {
vec3 diffuse;	//color of the oncoming light     //realistically, keep both specular and diffuse the same.
vec3 specular;  //color of the specular reflection
vec3 position;

};
struct sunLight {
vec3 direction;
vec3 diffuse;
vec3 specular;
};

struct coneLight {
vec3 position;
vec3 direction;
vec3 diffuse;
vec3 specular;
float cutoff_angle;    //in degrees
};

struct Material {
vec3 diffuse;	//reflected under diffuse light
vec3 specular;	//specular highlight color
float shine;  //should be a power of two : higher the value more "shinier" the material.

};


struct tMaterial {
sampler2D diffuse1;
sampler2D specular1;
float shine;
};

vec3 calculate_point(tMaterial tmaterial, pointLight light){		//calculates the lighting contribution of the point light
//diffuse : 
vec3 light_direction = normalize(light.position - FragPos);
vec3 norm = normalize(normal);
float diff = max(0.0, dot(light_direction, norm));
vec4 diffuse_color = texture(tmaterial.diffuse1, texCoords);
diffuse_color.xyz = gamma_correction(diffuse_color.xyz, 1.0f/2.2);
vec3 diffuse = light.diffuse * (diff * diffuse_color.xyz);

//specular :

vec3 reflected = reflect(-light_direction, norm);
vec3 viewDir = normalize(cameraPos - FragPos);
vec3 halfway = normalize(light_direction + viewDir);
float spec = pow(max(dot(norm, halfway), 0.0), tmaterial.shine);   //shininess
vec4 specular_color = texture(tmaterial.specular1, texCoords);
vec3 specular = light.specular * (spec * specular_color.xyz);
//finishing up
vec3 ambient = ambience * diffuse_color.xyz;
float d = distance(light.position, FragPos);
float denominator = 1.0f + 0.1f * d + 0.1f * pow(d,2);
float attenuation = 1.0f/denominator;
return  (diffuse * attenuation) + (specular * attenuation);
};

vec3 giveshine(pointLight light){
vec3 norm = normalize(normal);
vec3 viewDir = normalize(cameraPos - FragPos);
vec3 lightDir = normalize(light.position - FragPos);
vec3 halfway = normalize(viewDir + lightDir);
vec3 reflected = reflect(-lightDir, norm);
float spec = pow(max(dot(halfway, norm), 0.0f), 128.0f);
return vec3(spec * 0.5f);
};


vec3 calculate_sun(tMaterial tmaterial, sunLight light){
vec4 diffuse_color = texture(tmaterial.diffuse1, texCoords);
diffuse_color.xyz = gamma_correction(diffuse_color.xyz, 1.0f/2.2);
vec4 specular_color = texture(tmaterial.specular1, texCoords);
vec3 norm = normalize(normal);
vec3 light_direction = normalize(light.direction);
vec3 view_direction = normalize(cameraPos - FragPos);
vec3 halfway = normalize(view_direction - light_direction);

//diffuse :
float diff = max(0.0, dot(-light_direction, norm));
vec3 diffuse = light.diffuse * (diff * diffuse_color.xyz);
//specular : 
vec3 reflected = reflect(light_direction, norm);
float spec = pow(max(0.0, dot(halfway, norm)), tmaterial.shine);
vec3 specular = 0.01f * light.specular * (spec * specular_color.xyz);
//finishing up;
vec3 ambient =  ambience * diffuse_color.xyz;
return ambient + diffuse + specular;
};

vec3 calculate_cone(tMaterial tmaterial, coneLight light){

vec4 specular_color = texture(tmaterial.specular1, texCoords);
vec4 diffuse_color = texture(tmaterial.diffuse1, texCoords);
diffuse_color.xyz = gamma_correction(diffuse_color.xyz, 1.0f/2.2);
vec3 light_direction = normalize(light.position - FragPos);
vec3 viewDir = normalize(cameraPos - FragPos);
vec3 norm = normalize(normal);
vec3 light_normal = normalize(light.direction);
float costheta = cos(radians(light.cutoff_angle/2.0f));;
float cosalpha = max(dot(light_normal, -light_direction),0.0);
vec3 halfway = normalize(viewDir + light_direction);
if(cosalpha < costheta){
return vec3(0.0f);
};


//vertex falls inside the circle

float inner_circle = 0.95f * light.cutoff_angle;
//diffuse : 
float diff = max(0.0, dot(light_direction, norm));
vec3 diffuse = light.diffuse * (diff * diffuse_color.xyz);
//specular :
vec3 reflected = reflect(-light_direction, norm);
float spec = pow(max(dot(halfway, norm), 0.0), tmaterial.shine);   //shininess
vec3 specular = light.specular  * (spec * specular_color.xyz);
//finishing up
vec3 ambient = ambience * diffuse_color.xyz;
float d = distance(light.position, FragPos);
float denominator = 1.0f + 0.05f * d + 0.05f * pow(d,2);
float attenuation = 1.0f/denominator;
return (diffuse * attenuation) + (specular * attenuation);
//return vec3(1.0f);
};


uniform Material material;
pointLight point;
uniform tMaterial tmaterial;

sunLight sun;
coneLight cone;

//creating arrays for the lights
#define MAX_LIGHTS 8
uniform sunLight Suns[MAX_LIGHTS];
uniform pointLight Points[MAX_LIGHTS];
uniform coneLight Cones[MAX_LIGHTS];
uniform int pointCount;
uniform int sunCount;
uniform int coneCount;


void main(){         //NOTE : AMBIENT COLOR IS BEING ADDED OVER AND OVER WITH EACH LIGHT HENCE THAT IS SOMETHING THAT NEEDS TO BE TAKEN CARE OF. //done.
float gamma = 2.2f;
point.position = vec3(0.0f, 2.0f, 2.0f);
point.diffuse = vec3(1.0f,1.0f, 1.0f) * 0.5f;
point.specular = vec3(1.0f,1.0f, 1.0f) * 0.5f;

sun.direction = vec3(-3.0f, -3.0f, -6.0f);
sun.diffuse = vec3( 1.0f,1.0f, 0.9f);
sun.specular = vec3(1.0f, 1.0f, 1.0f);

cone.position = vec3(0.0f, 4.0f, -4.0f);
cone.direction = vec3(0.0f, -4.0f, 4.0f);
cone.diffuse = vec3(0.0f, 0.0f,3.0f) * 2;
cone.specular = vec3(0.0f, 0.0f,3.0f);
cone.cutoff_angle = 45.0f;


vec3 output_color = ambience * texture(tmaterial.diffuse1, texCoords).xyz;

for(int i = 0; i < pointCount ; i++){
output_color += calculate_point(tmaterial, Points[i]);
};
for(int i = 0; i < sunCount ; i++){
output_color += calculate_sun(tmaterial, Suns[i]);
};
for(int i = 0; i < coneCount ; i++){
output_color += calculate_cone(tmaterial, Cones[i]);
};


output_color += calculate_sun(tmaterial, Suns[0]);


FragColor = vec4(gamma_correction(output_color, gamma), 1.0f);

}