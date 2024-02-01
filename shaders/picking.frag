#version 330 core

uniform uint gObjectIndex;
uniform uint gDrawIndex;

out vec3 FragColor;

void main()
{
   FragColor = vec3(float(gObjectIndex), float(gDrawIndex),float(gl_PrimitiveID + 1));
}