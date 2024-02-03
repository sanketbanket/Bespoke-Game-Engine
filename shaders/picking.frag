#version 330 core

uniform uint gObjectIndex;
uniform uint gDrawIndex;

out uvec3 FragColor;

void main()
{
   FragColor = uvec3(gObjectIndex, gDrawIndex,gl_PrimitiveID + 1);
}