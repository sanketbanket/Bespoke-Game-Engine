#ifndef TEX_CLASS_H
#define TEX_CLASS_H

#include <glad/glad.h>
#include <string>
#include <stb/stb_image.h>
#include <iostream>

class Texture {
public : 
	GLuint ID;
	int unit;
	Texture(const char* imageFile, int texNumber);  
	void Bind();
	void Unbind();
	void Delete();

};





#endif