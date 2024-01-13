#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;


//layout (location = 2) in vec2 aTex;
//out vec2 texCoord;


out vec3 normal;
out vec3 FragPos;
out vec2 texCoords;

uniform float scale;
uniform mat4 model;
uniform mat4 cameraMatrix;



void main()
{
   gl_Position = cameraMatrix * model * vec4(aPos * scale , 1.0);
   FragPos = vec3(model * vec4(aPos, 1.0));
   normal = mat3(transpose(inverse(model))) * aNormal;
   texCoords = aTex;

   //texCoord = aTex;
}