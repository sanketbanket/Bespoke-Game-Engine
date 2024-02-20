#include <Windows.h>
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

string FileExplorerDialog(vector<GameObject*>& GameObjVec) {
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
		return fileName;
	}
	else {
		std::cout << "No file selected" << std::endl;
		return "";
	}
}

void DeleteObj(int& selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs, vector<Scene*>& scenes){
	int currsize = 0;
	if (selectedGameObj >= 0 && selectedItemIndex < GameObjVec.size()) {
		delete(GameObjVec[selectedItemIndex - currsize]);
		GameObjVec.erase(GameObjVec.begin() + selectedItemIndex - currsize);
		return;
	}
	currsize += GameObjVec.size();
	if (selectedGameObj >= currsize && selectedItemIndex < currsize + PointLightVec.size()) {
		cout << selectedItemIndex << " " << currsize << endl;
		delete(PointLightVec[selectedItemIndex - currsize]);
		PointLightVec.erase(PointLightVec.begin() + (selectedItemIndex - currsize));
		return;
	}
	currsize += PointLightVec.size();
	if (selectedGameObj >= currsize && selectedItemIndex < currsize + suns.size()) {
		delete(suns[selectedItemIndex - currsize]);
		suns.erase(suns.begin() + (selectedItemIndex - currsize));
		return;
	}
	currsize += suns.size();
	if (selectedGameObj >= currsize && selectedItemIndex < currsize + conevecs.size()) {
		delete(conevecs[selectedItemIndex - currsize]);
		conevecs.erase(conevecs.begin() + (selectedItemIndex - currsize));
		return;
	}
	currsize += conevecs.size();
	if (selectedGameObj >= currsize) {
		delete(scenes[selectedItemIndex - currsize]);
		scenes.erase(scenes.begin() + (selectedItemIndex - currsize));
		return;
	}
	currsize += scenes.size();
}

int DisplayObjectListAndGetIndex(int& selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs, vector<Scene*>& scenes) {
	//int selectedIndex = -1;
	int currsize = 0;
	// Display the list of objects
	if (ImGui::CollapsingHeader("Game Objects"))
	{
		for (int i = 0; i < GameObjVec.size(); ++i) {
			if (ImGui::Selectable(GameObjVec[i]->name.c_str(), selectedItemIndex == i)) {
				selectedItemIndex = i;
			}
		}
	}
	currsize += GameObjVec.size();
	if (ImGui::CollapsingHeader("Point Lights"))
	{
		for (int i = 0; i < PointLightVec.size(); ++i) {
			if (ImGui::Selectable(PointLightVec[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
		}
	}
	currsize += PointLightVec.size();
	if (ImGui::CollapsingHeader("Sun Lights"))
	{
		for (int i = 0; i < suns.size(); ++i) {
			if (ImGui::Selectable(suns[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
		}
	}
	currsize += suns.size();
	if (ImGui::CollapsingHeader("Cone Lights"))
	{
		for (int i = 0; i < conevecs.size(); ++i) {
			if (ImGui::Selectable(conevecs[i]->name.c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
		}
	}
	currsize += conevecs.size();
	if (ImGui::CollapsingHeader("Scenes"))
	{
		for (int i = 0; i < scenes.size(); ++i) {
			if (ImGui::Selectable(to_string(i).c_str(), selectedItemIndex == i + currsize)) {
				selectedItemIndex = i + currsize;
			}
		}
	}
	currsize += scenes.size();

	return selectedItemIndex;
}

void ImGuiConeLight(vector<ConeLight*>& conevecs, int selection) {
	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Columns(4);
		ImGui::Button("Cone light");   //diffuse,specular,strength,pos,dir,cutoff angle
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			conevecs[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &conevecs[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			conevecs[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &conevecs[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			conevecs[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &conevecs[selection]->Position.z, 0.1);
		ImGui::NextColumn();
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			conevecs[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &conevecs[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			conevecs[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &conevecs[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			conevecs[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &conevecs[selection]->Position.z, 0.1);
		ImGui::NextColumn();
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Strength", &conevecs[selection]->strength, 0.1);
		ImGui::Text("Cutoff angle");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Cutoff", &conevecs[selection]->Cutoff, 0.1);

		ImGui::Text("Diffuse");
		ImGui::SameLine();

		float myArray[3];
		float* ptr = reinterpret_cast<float*>(&conevecs[selection]->Diffuse);
		// Copy memory from vec to array
		std::memcpy(myArray, ptr, sizeof(glm::vec3));

		ImGui::ColorEdit3("Diffuse", myArray);
		ImGui::Text("Specular");
		ImGui::SameLine();

		ptr = reinterpret_cast<float*>(&conevecs[selection]->Specular);
		// Copy memory from vec to array
		std::memcpy(myArray, ptr, sizeof(glm::vec3));
		ImGui::ColorEdit4("Specular", myArray);
	}ImGui::End();
}
void ImGuiSunLight(vector<SunLight*>& suns, int selection) {
	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Sun light");   //diffuse,specular,strength,pos,dir
		ImGui::BeginChild("position", ImVec2(0, ImGui::GetWindowContentRegionMin().y));
		ImGui::Columns(4);

		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			suns[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &suns[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			suns[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &suns[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			suns[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &suns[selection]->Position.z, 0.1);
		ImGui::NextColumn();
		ImGui::EndChild();

		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Strength", &suns[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&suns[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&suns[selection]->Specular);
		ImGui::EndChild();
	}ImGui::End();
}
void ImguiPointLight(vector<PointLight*>& PointLightVec, int selection) {  //strength,position,diffuse,specular

	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Point light");
		ImGui::BeginChild("position",ImVec2(0,ImGui::GetWindowContentRegionMin().y));
		ImGui::Columns(4);
		//position]
		ImGui::Text("Position");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		if (ImGui::Button("X", buttonSize))
			PointLightVec[selection]->Position.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &PointLightVec[selection]->Position.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			PointLightVec[selection]->Position.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &PointLightVec[selection]->Position.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			PointLightVec[selection]->Position.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &PointLightVec[selection]->Position.z, 0.1);
		ImGui::NextColumn();
		ImGui::EndChild();
		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::NextColumn();
		ImGui::DragFloat("##Strength", &PointLightVec[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&PointLightVec[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&PointLightVec[selection]->Specular);
		ImGui::EndChild();
	}ImGui::End();

}
void ImguiGameObject(vector<GameObject*>& GameObjVec, int selection) {
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
			GameObjVec[selection]->tvecm.x = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X", &GameObjVec[selection]->tvecm.x, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->tvecm.y = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y", &GameObjVec[selection]->tvecm.y, 0.1);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->tvecm.z = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z", &GameObjVec[selection]->tvecm.z, 0.1);
		ImGui::NextColumn();
		//rotation
		ImGui::Text("Rotation");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selection]->xaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &GameObjVec[selection]->xaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->yaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &GameObjVec[selection]->yaxisanglem);
		ImGui::SameLine();

		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->zaxisanglem = 0;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &GameObjVec[selection]->zaxisanglem);
		ImGui::NextColumn();
		//scale
		ImGui::Text("Scale");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);

		ImGui::NextColumn();


		if (ImGui::Button("X", buttonSize))
			GameObjVec[selection]->svecm.x = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##X_scale", &GameObjVec[selection]->svecm.x, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Y", buttonSize))
			GameObjVec[selection]->svecm.y = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Y_scale", &GameObjVec[selection]->svecm.y, 0.05);
		ImGui::SameLine();

		ImGui::NextColumn();


		if (ImGui::Button("Z", buttonSize))
			GameObjVec[selection]->svecm.z = 1;


		ImGui::SameLine();
		ImGui::DragFloat("##Z_scale", &GameObjVec[selection]->svecm.z, 0.05);
	}
	ImGui::End();
}

void Gui(vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs, vector<Scene*>& scenes) {

	// feed inputs to dear imgui, start new frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// render your GUI
	if (ImGui::Begin("Heirarchy", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		selectedGameObj = DisplayObjectListAndGetIndex(selectedGameObj, GameObjVec, PointLightVec, suns, conevecs, scenes);
	}
	if (ImGui::Button("New Game Object")) {
		string fileName = FileExplorerDialog(GameObjVec);
		if (fileName != ""){
			string objname;
			cout << endl << "Enter Name: ";
			cin >> objname;
			GameObject* newObj = new GameObject(fileName, false, objname);
			GameObjVec.push_back(newObj);
		}
	}
	if (ImGui::Button("Delete Selected GameObject")) {
		DeleteObj(selectedGameObj, GameObjVec, PointLightVec, suns, conevecs, scenes);
		selectedGameObj--;
	}
	if (ImGui::Button("Add Scene")) {
		Scene* scene1 = new Scene;
		scenes.push_back(scene1);
	}
	if (ImGui::Button("Next Scene")) {
		selScene++;
		if (selScene >= scenes.size()) selScene = 0;
	}
	ImGui::End();
	
	int currsize = 0;
	if (selectedGameObj >= 0 && selectedGameObj < GameObjVec.size()) {
		ImguiGameObject(GameObjVec, selectedGameObj - currsize);
	}
	currsize += GameObjVec.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + PointLightVec.size()) {
		ImguiPointLight(PointLightVec, selectedGameObj - currsize);
	}
	currsize += PointLightVec.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + suns.size()) {
		cout << " " << selectedGameObj - currsize << " " << selectedGameObj << " " << currsize << endl;
		ImGuiSunLight(suns, selectedGameObj - currsize);
	}
	currsize += suns.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + conevecs.size()) {
		ImGuiConeLight(conevecs, selectedGameObj - currsize);
	}
	currsize += conevecs.size();
	if (selectedGameObj >= currsize) {
		cout << " " << selectedGameObj - currsize << " " << selectedGameObj << " " << currsize << endl;
		selScene = selectedGameObj - currsize;
	}
	currsize += scenes.size();

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

	saving_loading* sl_ins = new saving_loading();

	Shader diffuseShader("shaders/lighting.vert", "shaders/diffuse.frag"); // create shader
	//Shader shaderProg2("lighting.vert", "default.frag");
	Shader emissiveShader("shaders/emissive.vert", "shaders/emissive.frag");
	stbi_set_flip_vertically_on_load(true);

	SceneManager sceneManager;
	sceneManager.scenes = sl_ins->loading();

	float ambience = 0.0f;  //define the ambient light strength
	glm::vec3 ambient_light(1.0f, 1.0f, 1.0f);  //ambient light color
	ambient_light *= ambience;

	emissiveShader.Activate();

	GLuint modelID_2 = glGetUniformLocation(emissiveShader.ID, "model");
	GLuint transformID_2 = glGetUniformLocation(emissiveShader.ID, "cameraMatrix");

	diffuseShader.Activate();
	GLuint modelID = glGetUniformLocation(diffuseShader.ID, "model");     //grab the model and the camera transform matrices
	GLuint transformID = glGetUniformLocation(diffuseShader.ID, "cameraMatrix");

	Camera scenecam(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, (float)(width) / height, 0.1f, 100.0f);  //creating the camera
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);

	//making the lights
	PointLight point(glm::vec3(5.0f, 1.0f, 5.0f), glm::vec3(1.0f), glm::vec3(1.0f), "p1");
	vector<PointLight*> pointLights = {};
	pointLights.push_back(&point);

	SunLight sun(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "s1");
	vector<SunLight*> suns = {};
	suns.push_back(&sun);

	vector<ConeLight*> coneslights = {};

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
		//processTransformInputs(window, size(sceneManager.scenes[selScene]->gameObjects), selectedGameObj, sceneManager, selScene, scenecam, focus);
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

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scenecam.GetKeyInputs(window, 0.05f, focus);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 transform = scenecam.GetTransformMatrix();

		emissiveShader.Activate();
		glm::vec3 light_translation = glm::vec3(0.0f, 2.0f, 2.0f);
		glUniformMatrix4fv(modelID_2, 1, GL_FALSE, glm::value_ptr(glm::translate(model, light_translation)));

		glUniformMatrix4fv(transformID_2, 1, GL_FALSE, glm::value_ptr(transform));

		glm::vec3 light_pos = glm::vec3(0.0f) + light_translation;
		glm::vec3 light_color = glm::vec3(1.0f, 1.0f, 1.0f);

		RenderLights(emissiveShader, pointLights, suns);

		diffuseShader.Activate();

		diffuseShader.Setvec3("light.position", light_pos);
		diffuseShader.Setvec3("light.diffuse", light_color);
		diffuseShader.Setvec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		diffuseShader.Setvec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		diffuseShader.Setvec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		diffuseShader.Set1f("material.shine", 32.0f);

		PassPointsToShader(diffuseShader, pointLights);
		PassSunsToShader(diffuseShader, suns);

		GLuint lightID = glGetUniformLocation(diffuseShader.ID, "light_color");
		GLuint colorID = glGetUniformLocation(diffuseShader.ID, "mycolor");

		diffuseShader.Setvec3("ambience", ambient_light);
		diffuseShader.Setvec3("cameraPos", scenecam.Position);
		diffuseShader.Setvec3("lightColor", light_color);
		diffuseShader.Setvec3("lightpos", light_pos);
		diffuseShader.Set1f("tmaterial.shine", 64.0f);

		glUniformMatrix4fv(transformID, 1, GL_FALSE, glm::value_ptr(transform));

		sceneManager.switchToScene(selScene);
		sceneManager.renderCurrentScene(diffuseShader);

		if (!focus)
		{
			Gui(sceneManager.scenes[selScene]->gameObjects, pointLights, suns, coneslights, sceneManager.scenes);
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
	if (glfwGetKeyOnce(window, GLFW_KEY_1) == GLFW_PRESS and focus)
	{
		selectedGameObj = 0;
		selScene = 0;
	}
	if (glfwGetKeyOnce(window, GLFW_KEY_2) == GLFW_PRESS and focus)
	{
		selectedGameObj = 0;
		selScene = 1;
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