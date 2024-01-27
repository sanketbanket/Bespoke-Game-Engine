#pragma once
#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H


#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


std::string get_file_contents(const char* filename);


class Shader
{
public : 
	GLuint ID;
	Shader(const char* vertexFile, const char* FragmentFile);
	void Activate();
	void Delete();
	void Setvec3(const char* name, glm::vec3 vector);
	void Setmat4(const char* name, glm::mat4 matrix);
	void Set1f(const char* name, float value);
	void Set1i(const char* name, int value);
};
#endif