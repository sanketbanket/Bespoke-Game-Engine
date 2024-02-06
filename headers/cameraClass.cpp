#include "cameraClass.h"

static glm::mat3 make_rotation_matrix(float angleRad, glm::vec3 axis) {   //returns the rotation matrix for some angle in radians about some axis
	using namespace glm;
	axis = normalize(axis);
	vec3 col1, col2, col3;
	col1 = vec3(0.0f, axis.z, -axis.y);
	col2 = vec3(-axis.z, 0.0f, axis.x);
	col3 = vec3(axis.y, -axis.x, 0.0f);
	mat3 W(col1, col2, col3);
	return mat3(1.0f) + W * sin(angleRad) + W * W * (1 - cos(angleRad));

};

Camera::Camera(glm::vec3 position, glm::vec3 target, float fov, float aRatio, float nearest, float furthest) { //DO NOT GIVE ANY ELEVATION TO CAMERA IT IS YET TO BE FIXED
	using namespace glm;
	vec3 direction = normalize(position - target);
	vec3 right = normalize(cross( vec3(0.0f, 1.0f, 0.0f), direction));    //getting the local_right vector
	vec3 up = normalize(cross(direction, right));
	view = lookAt(position, target, up);  //generate the LookAt matrix
	projection = glm::perspective(glm::radians(fov), aRatio, nearest, furthest);
	Orientation = -direction;
	local_right = right;
	local_up = up;
	Position = position;

	fake_up = make_rotation_matrix(glm::angle(Orientation, vec3(Orientation.x, 0.0f, Orientation.z)), vec3(1.0f, 0.0f, 0.0f)) * vec3(0.0f, 1.0f, 0.0f);
	
	float fake_pitch = glm::degrees(glm::angle(fake_up, vec3(0.0f, 1.0f, 0.0f)));
	
	fake_right = normalize(cross(Orientation, fake_up));


	//std::cout << "fake_up " << fake_up.x << " " << fake_up.y << " " << fake_up.z << " " << std::endl;
	//std::cout << "fake_pitch " << fake_pitch << std::endl;

	//std::cout << "pitch_angle is " << glm::degrees(glm::angle(Orientation, vec3(Orientation.x, 0.0f, Orientation.z))) << std::endl;

	//std::cout <<"local_up " << local_up.x << " " << local_up.y << " " << local_up.z << " " << std::endl;
	//std::cout <<"local_right " << local_right.x << " " << local_right.y << " " << local_right.z << " " << std::endl;
	

}

glm::mat4 Camera::GetTransformMatrix(){
	return projection * view;
}




void Camera::ghetto_Mouse_Callback(GLFWwindow* window) { // ghetto cuz using right_mouse to enable mouse
	using namespace glm;
	//std::cout << "PREVIOUS MOUSE X AND Y : " << xposMouse << " " << yposMouse << std::endl;
	
	float sensitivity = 0.05;

	double new_xpos, new_ypos;
	glfwGetCursorPos(window, &new_xpos, &new_ypos);
	float xoffset = (new_xpos - xposMouse) * sensitivity;
	float yoffset = (new_ypos - yposMouse) * sensitivity;

	Orientation = make_rotation_matrix(radians(-xoffset), vec3(0.0f, 1.0f, 0.0f))  * Orientation;
	Orientation = make_rotation_matrix(radians(-yoffset), local_right) * Orientation;

	fake_up = make_rotation_matrix(radians(-yoffset), vec3(-1.0f, 0.0f, 0.0f)) * fake_up;
	fake_right = normalize(cross(vec3(0.0f, 0.0f, 1.0f), fake_up));

	local_right = normalize(cross(Orientation, vec3(0.0f, 1.0f, 0.0f)));
	local_up = normalize(cross(local_right, Orientation));
	

	//std::cout << "DIRECTION : " << Orientation.x << " " << Orientation.y << " " << Orientation.z << std::endl;
	//std::cout << "CAMERA RIGHT : " << local_right.x << "\\" << local_right.y << "\\" << local_right.z << std::endl;
	

	view = glm::rotate(glm::mat4(1.0f), radians(yoffset), vec3(1.0f, 0.0f, 0.0f)) * view;
	view = glm::rotate(glm::mat4(1.0f), radians(xoffset), fake_up) * view;
	//view = glm::rotate(glm::mat4(1.0f), radians(xoffset), vec3(0.0f, 1.0f, 0.0f)) * view;
	xposMouse = new_xpos;
	yposMouse = new_ypos;
		
	//std::cout << "MOUSE X AND Y : " << new_xpos << " " << new_ypos << std::endl;
		
	//std::cout << "MOUSE OFFSETS: " << xoffset << " " << yoffset << std::endl;
	

}


bool first = true;


void Camera::GetKeyInputs(GLFWwindow* window, float velocity, bool debug) {     //all the transformations will move the world itself inverse to the camera; 
	using namespace glm;
	vec3 front(0.0f, 0.0f, -1.0f);
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (first) { glfwGetCursorPos(window, &xposMouse, &yposMouse); first = false; }
	
	//if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {ghetto_Mouse_Callback(window);}
	ghetto_Mouse_Callback(window);

	//glfwSetCursorPosCallback(window, mouseCallback);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		view = translate(mat4(1.0f), -fake_up * velocity) * view;
		Position += velocity * vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		view = translate(mat4(1.0f), fake_up * velocity) * view;
		Position += velocity * vec3(0.0f, -1.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		view = translate(mat4(1.0f), -fake_right * velocity) * view;
		Position += -velocity * local_right;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		view = translate(mat4(1.0f), fake_right * velocity) * view;
		Position += velocity * local_right;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		view = translate(mat4(1.0f), -front * velocity) * view;
		Position += velocity * normalize(Orientation);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		view = translate(mat4(1.0f), front * velocity) * view;
		Position += -velocity * normalize(Orientation);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
		std::cout << "CAMERA POSITION : " << Position.x << '\\' << Position.y << '\\' << Position.z << std::endl;
	}
	
	
}
