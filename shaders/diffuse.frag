#version 330 core

uniform vec3 ambience;

in vec3 normal;		//mesh normal vector
in vec3 FragPos;	//position vector for the vertex
in vec2 texCoords;	//texture coordinate for the vertex

out vec4 FragColor;

uniform vec3 cameraPos;

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
sampler2D diffuse;
sampler2D specular;
float shine;
};

vec3 calculate_point(tMaterial tmaterial, pointLight light){		//calculates the lighting contribution of the point light
//diffuse : 
vec3 light_direction = normalize(light.position - FragPos);
vec3 norm = normalize(normal);
float diff = max(0.0, dot(light_direction, norm));
vec4 diffuse_color = texture(tmaterial.diffuse, texCoords);
vec3 diffuse = light.diffuse * (diff * diffuse_color.xyz);

//specular :

vec3 reflected = reflect(-light_direction, norm);
vec3 viewDir = normalize(cameraPos - FragPos);
vec3 halfway = normalize(light_direction + viewDir);
float spec = pow(max(dot(norm, halfway), 0.0), tmaterial.shine);   //shininess
vec4 specular_color = texture(tmaterial.specular, texCoords);
vec3 specular = light.specular  * (spec * specular_color.xyz);
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
vec4 diffuse_color = texture(tmaterial.diffuse, texCoords);
vec4 specular_color = texture(tmaterial.specular, texCoords);
vec3 norm = normalize(normal);
vec3 light_direction = normalize(light.direction);
vec3 view_direction = normalize(cameraPos - FragPos);
//diffuse :
float diff = max(0.0, dot(-light_direction, norm));
vec3 diffuse = light.diffuse * (diff * diffuse_color.xyz);
//specular : 
vec3 reflected = reflect(light_direction, norm);
float spec = pow(max(0.0, dot(view_direction, reflected)), tmaterial.shine);
vec3 specular = light.specular * (spec * specular_color.xyz);
//finishing up;
vec3 ambient =  ambience * diffuse_color.xyz;
return ambient + diffuse + specular;
};

vec3 calculate_cone(tMaterial tmaterial, coneLight light){

vec4 specular_color = texture(tmaterial.specular, texCoords);
vec4 diffuse_color = texture(tmaterial.diffuse, texCoords);
vec3 light_direction = normalize(light.position - FragPos);
vec3 viewDir = normalize(cameraPos - FragPos);
vec3 norm = normalize(normal);
vec3 light_normal = normalize(light.direction);
float costheta = cos(radians(light.cutoff_angle/2.0f));;
float cosalpha = max(dot(light_normal, -light_direction),0.0);

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
float spec = pow(max(dot(viewDir, reflected), 0.0), tmaterial.shine);   //shininess
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

void main(){         //NOTE : AMBIENT COLOR IS BEING ADDED OVER AND OVER WITH EACH LIGHT HENCE THAT IS SOMETHING THAT NEEDS TO BE TAKEN CARE OF. //done.

point.position = vec3(0.0f, 2.0f, 2.0f);
point.diffuse = vec3(1.0f,1.0f, 1.0f);
point.specular = vec3(1.0f,1.0f, 1.0f);

sun.direction = vec3(-3f, -3f, -6f);
sun.diffuse = vec3( 1.0f,1.0f, 0.0f);
sun.specular = vec3(1.0f, 1.0f, 1.0f);

cone.position = vec3(0.0f, 4.0f, -4.0f);
cone.direction = vec3(0.0f, -4.0f, 4.0f);
cone.diffuse = vec3(0.0f, 0.0f,3.0f) * 2;
cone.specular = vec3(0.0f, 0.0f,3.0f);
cone.cutoff_angle = 45.0f;


vec3 output_color = ambience * texture(tmaterial.diffuse, texCoords).xyz;

//output_color += calculate_point(tmaterial, point);
//output_color += calculate_sun(tmaterial, sun);
output_color += calculate_cone(tmaterial, cone);
//output_color += giveshine(point);

FragColor = vec4(output_color, 1.0f);

}