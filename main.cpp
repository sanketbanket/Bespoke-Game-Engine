#include<Windows.h>
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
#include "headers/light_objects.h"
#include "headers/GameObj.h"
#include "headers/Scene.h"
#include "headers/Saving_Loading.h"
#include <vector>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <imgui_internal.h>
#pragma comment(lib, "OpenGL32.lib")

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)				\
	(glfwGetKey(WINDOW, KEY) ?				\
	 (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :	\
	 (keyOnce[KEY] = false))

int selectedGameObj = 0;
int selScene = 0;

void processTransformInputs(GLFWwindow* window, int GOAsize, int SelObj, SceneManager sceneManager, int SelScene, Camera scenecam, bool& focus);

void FileExplorerDialog(vector<GameObject*>& GameObjVec) {
	// Initialize a structure for the file dialog
	OPENFILENAME ofn;
	wchar_t szFile[260] = { 0 }; // Buffer for file name

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = (LPCWSTR)"All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	// Display the Open dialog box
	if (GetOpenFileName(&ofn) == TRUE) {
		int len = WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, NULL, 0, NULL, NULL);
		std::string fileName(len, 0);
		WideCharToMultiByte(CP_UTF8, 0, ofn.lpstrFile, -1, &fileName[0], len, NULL, NULL);
		std::cout << "Selected file: " << fileName << std::endl;
		for (int i = 0; i < size(GameObjVec); i++) {
			cout << GameObjVec[i]->name;
		}
		string objname;
		cin >> objname;
		GameObject* newObj = new GameObject(fileName, false, objname);
		GameObjVec.push_back(newObj);
	}
	else {
		std::cout << "No file selected" << std::endl;
	}
}

int DisplayObjectListAndGetIndex(vector<GameObject*>& GameObjVec, int& selectedItemIndex) {
	//int selectedIndex = -1;

	// Display the list of objects
	for (int i = 0; i < GameObjVec.size(); ++i) {
		if (ImGui::Selectable(GameObjVec[i]->name.c_str(), selectedItemIndex == i)) {
			selectedItemIndex = i;
		}
	}
	if (ImGui::Button("Open File Explorer")) {
		FileExplorerDialog(GameObjVec);
	}

	return selectedItemIndex;
}
void Gui(vector<GameObject*>& GameObjVec) {

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// render your GUI
	if (ImGui::Begin("heirarchy", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		selectedGameObj = DisplayObjectListAndGetIndex(GameObjVec, selectedGameObj);
	}
	ImGui::End();
	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Transform", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Columns(4);
		//position
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			GameObjVec[selectedGameObj]->tvecm.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &GameObjVec[selectedGameObj]->tvecm.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selectedGameObj]->tvecm.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &GameObjVec[selectedGameObj]->tvecm.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selectedGameObj]->tvecm.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &GameObjVec[selectedGameObj]->tvecm.z, 0.1);
		ImGui::NextColumn();
		//rotation
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selectedGameObj]->xaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &GameObjVec[selectedGameObj]->xaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selectedGameObj]->yaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &GameObjVec[selectedGameObj]->yaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selectedGameObj]->zaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &GameObjVec[selectedGameObj]->zaxisanglem);
		ImGui::NextColumn();
		//scale
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selectedGameObj]->svecm.x = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##X_scale", &GameObjVec[selectedGameObj]->svecm.x, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selectedGameObj]->svecm.y = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_scale", &GameObjVec[selectedGameObj]->svecm.y, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selectedGameObj]->svecm.z = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_scale", &GameObjVec[selectedGameObj]->svecm.z, 0.05);
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main() {
	const int width = 1920;
	const int height = 1080;
	//creating window
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWmonitor* a = glfwGetPrimaryMonitor();
	GLFWwindow* window = glfwCreateWindow(width, height, "LIGHTING TESTS", NULL, NULL);

	if (window == NULL) {
		std::cout << "FAILED TO CREATE WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	};
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, width, height);

	glEnable(GL_MULTISAMPLE);


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
	saving_loading* sl_ins = new saving_loading();

	Shader diffuseShader("shaders/lighting.vert", "shaders/diffuse.frag"); // create shader
	//Shader shaderProg2("lighting.vert", "default.frag");
	Shader emissiveShader("shaders/emissive.vert", "shaders/emissive.frag");
	stbi_set_flip_vertically_on_load(true);

	SceneManager sceneManager;

	/*Scene* scene0 = new Scene;
	Scene* scene1 = new Scene;

	GameObject* bag = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	GameObject* rock = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
	GameObject* skull = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");

	GameObject* bag1 = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	GameObject* rock1 = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
	GameObject* skull1 = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");

	scene0->addGameObject(bag, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene0->addGameObject(rock, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene0->addGameObject(skull, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	
	scene1->addGameObject(bag1, glm::vec3(2.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 0.3f);
	scene1->addGameObject(rock1, glm::vec3(-2.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	scene1->addGameObject(skull1, glm::vec3(0.0f, 0.0f, 2.0f), 0.0f, 0.0f, 0.0f, 5.0f);

	sceneManager.addScene(scene0);
	sceneManager.addScene(scene1);*/

	sceneManager.scenes = sl_ins->loading();

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

	float ambience = 0.0f;  //define the ambient light strength
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
	
	SunLight sun(glm::vec3(0.0f, -1.0f, 1.0f), 0.0f*glm::vec3(1.0f, 1.0f, 0.8f), 0.0f*glm::vec3(1.0f, 1.0f, 0.8f));
	vector<SunLight*> suns = {};
	suns.push_back(&sun);
	
	vector<PointLight*> pointLights;
	pointLights.push_back(&point);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330 core");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	//rendering loop 

	bool focus = true;

	while (!glfwWindowShouldClose(window)) {

		//float time = glfwGetTime();
		float time = 0.0f;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { break; }   //setting up the close window button
		processTransformInputs(window, size(sceneManager.scenes[selScene]->gameObjects), selectedGameObj, sceneManager, selScene, scenecam, focus);

		float scale = 1.0f;
		emissiveShader.Set1f("scale", 1.0f);

		glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scenecam.GetKeyInputs(window, 0.05f, focus);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 transform = scenecam.GetTransformMatrix();

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

		RenderLights(emissiveShader, pointLights, suns);

		
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

		diffuseShader.Setmat4("model", glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f)));
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
		
		sceneManager.switchToScene(selScene);
		sceneManager.renderCurrentScene(diffuseShader);
		
		if (!focus)
		{
			Gui(sceneManager.scenes[selScene]->gameObjects);
		}
		

		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	//cube_vao.Delete();
	//cube_vbo.Delete();
	//cube_ebo.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();

	sl_ins->saving(sceneManager.scenes);

	for (Scene* obj : sceneManager.scenes) {
		delete obj;
	}

	return 0;
}

void processTransformInputs(GLFWwindow* window, int GOVsize, int SelObj, SceneManager sceneManager, int SelScene, Camera scenecam, bool& focus)
{
	GameObject* selectedGO = sceneManager.scenes[sceneManager.currentSceneIndex]->gameObjects[SelObj];
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { selectedGO->tvecm[1] = selectedGO->tvecm[1] + 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { selectedGO->tvecm[1] = selectedGO->tvecm[1] - 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { selectedGO->tvecm[0] = selectedGO->tvecm[0] + 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { selectedGO->tvecm[0] = selectedGO->tvecm[0] - 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) { selectedGO->tvecm[2] = selectedGO->tvecm[2] + 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) { selectedGO->tvecm[2] = selectedGO->tvecm[2] - 0.03f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { selectedGO->xaxisanglem = selectedGO->xaxisanglem + 1.0f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { selectedGO->xaxisanglem = selectedGO->xaxisanglem - 1.0f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { selectedGO->yaxisanglem = selectedGO->yaxisanglem + 1.0f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { selectedGO->yaxisanglem = selectedGO->yaxisanglem - 1.0f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { selectedGO->zaxisanglem = selectedGO->zaxisanglem + 1.0f; }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS and glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { selectedGO->zaxisanglem = selectedGO->zaxisanglem - 1.0f; }
	if (glfwGetKeyOnce(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		selectedGameObj++;
		if (selectedGameObj >= GOVsize) selectedGameObj = 0;
	}
	if (glfwGetKeyOnce(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		selScene = 0;
	}
	if (glfwGetKeyOnce(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		selScene = 1;
	}
	if (glfwGetKeyOnce(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		GameObject* x = new GameObject("Models/cwire/sword.obj", false,"sword");
		sceneManager.scenes[sceneManager.currentSceneIndex]->addGameObject(x);
	}
	if (glfwGetKeyOnce(window, GLFW_KEY_G) == GLFW_PRESS) {
		focus = !focus;
		if (focus)
		{
			glfwSetCursorPos(window, scenecam.xposMouse, scenecam.yposMouse);
		}
		else {
			//glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
		}
		cout << focus;
	}
}