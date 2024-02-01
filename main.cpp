#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include "headers/shaderClass.h"
#include "headers/VAO.h"
#include "headers/VBO.h"
#include "headers/EBO.h"
#include "headers/textureClass.h"
#include "headers/Model.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "headers/cameraClass.h"
#include <vector>
#include "headers/light_objects.h"
#include "picking/picking.h"
#include "picking/technique.h"


void PickingPhase(glm::mat4 model, glm::mat4 transform, Model ourModel, PickingTexture m_pickingTexture, PickingTechnique m_pickingEffect) {
	m_pickingTexture.EnableWriting();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_pickingEffect.Enable();

	//for (uint i = 0; i < (int)ARRAY_SIZE_IN_ELEMENTS(m_worldPos); i++) {
	//	worldTransform.SetPosition(m_worldPos[i]);
	//	// Background is zero, the real objects start at 1
	//	m_pickingEffect.SetObjectIndex(i + 1);
	//	glm::mat4 WVP = transform*model;
	//	m_pickingEffect.SetWVP(WVP);
	//	pMesh->Render(&m_pickingEffect);
	//}
	m_pickingEffect.SetObjectIndex(1);
	glm::mat4 WVP = transform * model;
	m_pickingEffect.SetWVP(WVP);
	ourModel.Draw(m_pickingEffect);
	m_pickingTexture.DisableWriting();
}


int main() {
	const int width = 1000;
	const int height = 1000;
	//creating window
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(width, height, "LIGHTING TESTS", NULL, NULL);

	if (window == NULL) {
		std::cout << "FAILED TO CREATE WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	};
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);




	GLfloat cube_verts[] = {  //coords      //normals             //texture coordinates 
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};	
	GLuint cube_indices[36];
	for (int i = 0; i < 36; i++) {
		cube_indices[i] = i;
	}
	


	GLfloat square[] = {
		0.5f, 0.5f, 0.0f,         0.3f, 0.3f, 0.3f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,			0.3f, 0.3f, 0.3f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,			0.3f, 0.3f, 0.3f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f,			0.3f, 0.3f, 0.3f, 0.0f, 0.0f

	};

	GLuint indices[] = {
		0,1,2,
		0,3,2
	};

	float floor[] = {
		0.5f, 0.0, 0.5f,  0.0f, 1.0f, 0.0f,	0.0f, 0.0f,
		-0.5f, 0.0, 0.5f, 0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		-0.5f, 0.0, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.0, -0.5f, 0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	};

	Shader diffuseShader("shaders/lighting.vert", "shaders/diffuse.frag"); // create shader
	//Shader shaderProg2("lighting.vert", "default.frag");
	Shader emissiveShader("shaders/emissive.vert", "shaders/emissive.frag");
	stbi_set_flip_vertically_on_load(true);
	Model ourModel1("Models/bag_model/backpack.obj");


	GLuint vao;                                   //creating the buffer data for the Triangle
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));  
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	


	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//now for the cube

	GLuint cube_vao;
	glGenVertexArrays(1, &cube_vao);
	glBindVertexArray(cube_vao);

	GLuint cube_vbo;
	glGenBuffers(1, &cube_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

	GLuint cube_ebo;
	glGenBuffers(1, &cube_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	//cube done

	//floor now : 
	GLuint floor_vao;
	glGenVertexArrays(1, &floor_vao);
	glBindVertexArray(floor_vao);

	GLuint floor_vbo;
	glGenBuffers(1, &floor_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);

	GLuint floor_ebo;
	glGenBuffers(1, &floor_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	

	//floor done

	float ambience = 0.2f;  //define the ambient light strength
	glm::vec3 ambient_light(1.0f, 1.0f, 1.0f);  //ambient light color
	ambient_light *= ambience;

	emissiveShader.Activate();

	GLuint modelID_2 = glGetUniformLocation(emissiveShader.ID, "model");
	GLuint transformID_2 = glGetUniformLocation(emissiveShader.ID, "cameraMatrix");

	diffuseShader.Activate();
	GLuint modelID = glGetUniformLocation(diffuseShader.ID, "model");     //grab the model and the camera transform matrices
	GLuint transformID = glGetUniformLocation(diffuseShader.ID, "cameraMatrix");

	Texture container_tex("textures/container_diffuse.png", 0);
	Texture container_spectex("textures/container_specular.png", 1);

	container_tex.Unbind();
	container_spectex.Unbind();

	Texture floor_tex("textures/floor_diffuse.jpg",0);
	floor_tex.Unbind();
	Texture floor_spectex("textures/floor_specular.jpg", 1);
	floor_spectex.Unbind();


	Camera scenecam(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, (float)(width) / height, 0.1f, 100.0f);  //creating the camera
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);
	
	//making the lights
	PointLight point(glm::vec3(5.0f, 1.0f, 5.0f), glm::vec3(1.0f), glm::vec3(1.0f));
	
	SunLight sun(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f));
	vector<SunLight*> suns = {};
	suns.push_back(&sun);
	
	vector<PointLight*> pointLights;
	pointLights.push_back(&point);

	//initialize picking
	Shader pickingShader("shaders/picking.vert", "shaders/picking.frag");
	PickingTexture m_pickingTexture;
	PickingTechnique m_pickingEffect(pickingShader);
	m_pickingEffect.Init();
	m_pickingTexture.Init(width, height);
	double mouse_x, mouse_y;

	//rendering loop 

	while (!glfwWindowShouldClose(window)) {

		//float time = glfwGetTime();
		float time = 0.0f;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { break; }   //setting up the close window button

		

		float scale = 1.0f;
		emissiveShader.Set1f("scale", 1.0f);

		glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scenecam.GetKeyInputs(window, 0.05f, true);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 transform = scenecam.GetTransformMatrix();

		PickingPhase(glm::scale(glm::translate(glm::mat4(1.f), glm::vec3(1.0f, 0.0f, 0.0f)), glm::vec3(0.6f, 0.6f, 0.6f)), transform, ourModel1, m_pickingTexture, m_pickingEffect);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
			glfwGetCursorPos(window, &mouse_x, &mouse_y);
			cout << mouse_x << " " << mouse_y << endl;
			PickingTexture::PixelInfo Pixel = m_pickingTexture.ReadPixel(mouse_x,
				height - mouse_y - 1);
			Pixel.Print();
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		emissiveShader.Activate();
		glm::vec3 light_translation = glm::vec3(0.0f, 2.0f, 2.0f);
		glUniformMatrix4fv(modelID_2, 1, GL_FALSE, glm::value_ptr(glm::translate(model, light_translation)));

		
		glUniformMatrix4fv(transformID_2, 1, GL_FALSE, glm::value_ptr(transform));
		
		glBindVertexArray(vao);
	
		glm::vec3 light_pos = glm::vec3(0.0f) + light_translation;
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);


		//drawing the square
		
		
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		emissiveShader.Setmat4("model", glm::translate(model, glm::vec3(0.0f, 4.0f, -4.0f)));
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);


		
		diffuseShader.Activate();

		container_tex.Bind();
		glUniform1i(glGetUniformLocation(diffuseShader.ID, "tmaterial.diffuse1"), 0);
		container_spectex.Bind();
		glUniform1i(glGetUniformLocation(diffuseShader.ID, "tmaterial.specular1"), 1);

		diffuseShader.Setvec3("light.position", light_pos);
		diffuseShader.Setvec3("light.diffuse", light_color);
		diffuseShader.Setvec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		diffuseShader.Setvec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		diffuseShader.Setvec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		diffuseShader.Set1f("material.shine", 32.0f);
		//diffuseShader.Setvec3("Points[0].position", point.Position);
		//diffuseShader.Setvec3("Points[0].diffuse", point.Diffuse);
		//diffuseShader.Setvec3("Points[0].specular", point.Specular);
		//ApplyPointToShader(diffuseShader, point, 0);
		PassPointsToShader(diffuseShader, pointLights); 
		PassSunsToShader(diffuseShader, suns);


		//GLuint lightID = glGetUniformLocation(diffuseShader.ID, "light_color");
		GLuint colorID = glGetUniformLocation(diffuseShader.ID, "mycolor");
		//glUniform3fv(lightID, 1, glm::value_ptr(light_color));


		diffuseShader.Setvec3("ambience", ambient_light);
		diffuseShader.Setvec3("cameraPos", scenecam.Position);
		diffuseShader.Setvec3("lightColor", light_color);
		diffuseShader.Setvec3("lightpos", light_pos);
		
		
		
		diffuseShader.Set1f("tmaterial.shine", 64.0f);
		//animating the cube a bit;

		diffuseShader.Setmat4("model", glm::rotate(model, (float)glm::radians(time* 20.0), glm::vec3(1.0f, 0.0f, 0.5f)));
		
		glBindVertexArray(cube_vao);    //drawing the cube
		glUniformMatrix4fv(transformID, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(int), GL_UNSIGNED_INT, 0);

		diffuseShader.Setmat4("model", glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f)));
		glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		diffuseShader.Setmat4("model", glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f)));
		glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		//drawing the floor
		
		glBindVertexArray(floor_vao);
		
		diffuseShader.Setmat4("model", glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)) * glm::scale(model, glm::vec3(20.0f)));
		
		floor_tex.Bind();
		floor_spectex.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		
		model = glm::mat4(1.f);
		model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f)); // translate it right
		model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));	// it's a bit too big for our scene, so scale it down
		diffuseShader.Setmat4("model", model);
		ourModel1.Draw(diffuseShader);
		
		
		
		
		

		glfwSwapBuffers(window);
		glfwPollEvents();

	}



	//cube_vao.Delete();
	//cube_vbo.Delete();
	//cube_ebo.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();




	return 0;
}