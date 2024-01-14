#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/gtx/vector_angle.hpp"

class Camera {				
public: 

	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);   // position vector and direction vector
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);  
	glm::mat4 projection = glm::mat4(1.0f);

	glm::vec3 local_right, local_up;
	glm::vec3 fake_up, fake_right;
	double xposMouse, yposMouse;

	Camera(glm::vec3 position, glm::vec3 target, float fov, float aRatio, float nearest, float furthest);
	glm::mat4 GetTransformMatrix();
	void GetKeyInputs(GLFWwindow* window, float velocity, bool debug);
	void ghetto_Mouse_Callback(GLFWwindow* window);
	

	bool take_first_mouse_location = true;
	
};



#endif