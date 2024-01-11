#include "cameraClass.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, float fov, float aRatio, float nearest, float furthest) {
	using namespace glm;
	vec3 direction = position - target;
	vec3 right = cross( vec3(0.0f, 1.0f, 0.0f), direction);    //getting the local_right vector
	vec3 up = cross(direction, right);
	view = lookAt(position, target, up);  //generate the LookAt matrix
	projection = glm::perspective(glm::radians(fov), aRatio, nearest, furthest);

	CurrentPosition = position;
	Orientation = normalize(direction);
	local_right = normalize(right);   
	local_up = normalize(up);
	
	using namespace std;
	cout << "THE CURRENT POSITION IS : " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
	cout << "THE LOCAL UP DIRECTION IS : " << local_up.x<<" " << local_up.y <<" " << local_up.z << std::endl;
	cout << "THE LOCAL RIGHT DIRECTION IS : " << local_right.x << " " << local_right.y << " " << local_right.z << std::endl;
	

}

glm::mat4 Camera::GetTransformMatrix(){
	return projection * view;
}




void Camera::ghetto_Mouse_Callback(GLFWwindow* window) { // ghetto cuz using right_mouse to enable mouse
	
	//std::cout << "PREVIOUS MOUSE X AND Y : " << xposMouse << " " << yposMouse << std::endl;
	
	float sensitivity = 0.05;

	double new_xpos, new_ypos;
	glfwGetCursorPos(window, &new_xpos, &new_ypos);
	float xoffset = (new_xpos - xposMouse) * sensitivity;
	float yoffset = (new_ypos - yposMouse) * sensitivity;

	
	
	glm::mat4 pitch = glm::rotate(glm::mat4(1.0f), glm::radians(-yoffset), local_right);
	view = pitch * view;

	glm::mat4 yaw = glm::rotate(glm::mat4(1.0f), glm::radians(xoffset), local_up);
	view = yaw * view;




	xposMouse = new_xpos;
	yposMouse = new_ypos;
		
	//std::cout << "MOUSE X AND Y : " << new_xpos << " " << new_ypos << std::endl;
		
	//std::cout << "MOUSE OFFSETS: " << xoffset << " " << yoffset << std::endl;
	

}


void Camera::GetKeyInputs(GLFWwindow* window, float velocity, bool debug) {     //all the transformations will move the world itself inverse to the camera; 
	//hence all the directions are reversed
	using namespace glm;
	vec3 forward = cross(local_up, local_right);
	bool useMouse = false;

	/*
	
	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) {    //enable mouse
		useMouse = true;
		if (debug) { std::cout << "Mouse Active \n"; }
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {     //disable mouse
		useMouse = false;
		if (debug) { std::cout << "Mouse Inactive\n"; }

	}
	*/

	/*
	if (useMouse) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window, Mouse_Callback);       //glfw won't accept callback function :(

	}
	*/
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!take_first_mouse_location) {
		//std::cout << "now using mouse \n";
		
		
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {


			//glfwGetCursorPos(window, &xposMouse, &yposMouse);
			ghetto_Mouse_Callback(window);
		}

		
	}
	else {
		take_first_mouse_location = false;
		glfwGetCursorPos(window, &xposMouse, &yposMouse);

	}

	//camera translations henceforth

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {    //camera forward
		if (debug) {
			std::cout << "KEY S is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}
		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), -forward * velocity) * view;
		}
		CurrentPosition += velocity * -forward;
	}
	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {     //camera left
		if (debug) {
			std::cout << "KEY D is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}

		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), local_right * velocity) * view;
		}
		CurrentPosition += velocity * local_right;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {    //camera backward
		if (debug) {
			std::cout << "KEY W is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}
		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), forward * velocity) * view;
		}
		CurrentPosition += velocity * forward;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {    //camera right
		if (debug) {
			std::cout << "KEY A is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}
		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), -local_right * velocity) * view;
		}
		CurrentPosition += velocity * -local_right;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {    //camera up
		if (debug) {
			std::cout << "KEY D is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}
		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), -local_up * velocity) * view;
		}
		CurrentPosition += velocity * local_up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {    //camera down
		if (debug) {
			std::cout << "KEY D is pressed!" << std::endl;
			std::cout << "X , Y , Z " << CurrentPosition.x << " " << CurrentPosition.y << " " << CurrentPosition.z << std::endl;
		}
		if (Orientation.x == 0 && Orientation.y == 0) {
			view = translate(mat4(1.0f), local_up * velocity) * view;
		}
		CurrentPosition += velocity * -local_up;
	}
}
