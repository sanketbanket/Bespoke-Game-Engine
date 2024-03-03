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
#include <imgui/misc/cpp/imgui_stdlib.h>
#pragma comment(lib, "OpenGL32.lib")

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)				\
	(glfwGetKey(WINDOW, KEY) ?				\
	 (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :	\
	 (keyOnce[KEY] = false))

int selectedGameObj = -1;
int selScene = 0;
string objname;string fileName;

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
		filesystem::path absPath(fileName);
		filesystem::path currPath = filesystem::current_path();
		// Find common root directory
		auto absIter = absPath.begin();
		auto currIter = currPath.begin();
		while (absIter != absPath.end() && currIter != currPath.end() && *absIter == *currIter) {
			++absIter;
			++currIter;
		}

		// Construct relative path
		filesystem::path relativePath;
		for (; currIter != currPath.end(); ++currIter) {
			relativePath /= "..";
		}
		for (; absIter != absPath.end(); ++absIter) {
			relativePath /= *absIter;
		}

		std::cout << "Selected file: " << fileName <<" relative"<<relativePath.string()<< std::endl;
		return relativePath.string();
	}
	else {
		std::cout << "No file selected" << std::endl;
		return "";
	}
}


void DeleteObj(int selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs, vector<Scene*>& scenes) {
	int currsize = 0;

	if (selectedItemIndex >= currsize && selectedItemIndex < GameObjVec.size()) {
		GameObjVec.erase(GameObjVec.begin() + selectedItemIndex);
		return;
	}

	currsize += GameObjVec.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + PointLightVec.size()) {
		PointLightVec.erase(PointLightVec.begin() + (selectedItemIndex - currsize));
		return;
	}

	currsize += PointLightVec.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + suns.size()) {
		suns.erase(suns.begin() + (selectedItemIndex - currsize));
		return;
	}

	currsize += suns.size();

	if (selectedItemIndex >= currsize && selectedItemIndex < currsize + conevecs.size()) {
		conevecs.erase(conevecs.begin() + (selectedItemIndex - currsize));
		return;
	}
}

void AddGameObj(vector<GameObject*>& GameObjVec) {
	if (ImGui::Button("+")) {
		fileName = FileExplorerDialog(GameObjVec);
		if (fileName != "") {
			objname = "";
			ImGui::OpenPopup("NewObject");
		}
	}
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewObject", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("FilePath: %s", fileName.c_str());
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				GameObject* newObj = new GameObject(fileName, false, objname);
				GameObjVec.push_back(newObj);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}
}

int DisplayObjectListAndGetIndex(int& selectedItemIndex, vector<GameObject*>& GameObjVec, vector<PointLight*>& PointLightVec, vector<SunLight*>& suns, vector<ConeLight*>& conevecs, vector<Scene*>& scenes) {
	//int selectedIndex = -1;
	char buf[64];
	int currsize = 0;
	// Display the list of objects
	bool op=ImGui::CollapsingHeader("Game Objects");
	ImGui::SameLine(ImGui::GetWindowWidth() - 30);
	//ImGui::AlignTextToFramePadding();
	AddGameObj(GameObjVec);
	if (op)
	{
		for (int i = 0; i < GameObjVec.size(); ++i) {
			if (ImGui::Selectable(GameObjVec[i]->name.c_str(), selectedItemIndex == i)) {
				selectedItemIndex = i;
			}
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Game Object: %s", GameObjVec[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs, scenes);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
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
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Point Light: %s", PointLightVec[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs, scenes);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
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
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Sun Light: %s", suns[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs, scenes);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
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
			if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
			{
				selectedItemIndex = i + currsize;
				sprintf_s(buf, "Delete Cone Light: %s", conevecs[i]->name.c_str());
				if (ImGui::Button(buf)) {
					DeleteObj(selectedItemIndex, GameObjVec, PointLightVec, suns, conevecs, scenes);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
	}

	return selectedItemIndex;
}
void AddPointPopup(vector<PointLight*>& PointLightVec) {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewPoint", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				PointLight* p_temp = new PointLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f), glm::vec3(1.0f), objname);
				PointLightVec.push_back(p_temp);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

}

void ImGuiConeLight(vector<ConeLight*>& conevecs, int selection) {
	// render your GUI

	ImGui::SetNextWindowSize(ImVec2(500, 500));
	if (ImGui::Begin("Light Properties", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Button("Cone light");   //diffuse,specular,strength,pos,dir,cutoff angle
		ImGui::BeginChild("position", ImVec2(0, ImGui::GetWindowContentRegionMin().y*2));
		ImGui::Columns(4);

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

		ImGui::Text("Direction");
		ImGui::SameLine();
		ImGui::SetColumnWidth(0, 65);
		ImGui::NextColumn();

		if (ImGui::Button("X", buttonSize))
			conevecs[selection]->Direction.x = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##X_angle", &conevecs[selection]->Direction.x, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Y", buttonSize))
			conevecs[selection]->Direction.y = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Y_angle", &conevecs[selection]->Direction.y, 0.01);
		ImGui::SameLine();
		ImGui::NextColumn();

		if (ImGui::Button("Z", buttonSize))
			conevecs[selection]->Direction.z = 0;
		ImGui::SameLine();
		ImGui::DragFloat("##Z_angle", &conevecs[selection]->Direction.z, 0.01);
		ImGui::NextColumn();
		ImGui::EndChild();

		ImGui::BeginChild("others");
		ImGui::Columns(2);
		ImGui::Text("Strength");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::SetColumnWidth(0, 65);
		ImGui::DragFloat("##Strength", &conevecs[selection]->strength, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Cutoff angle");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::DragFloat("##Cutoff", &conevecs[selection]->Cutoff, 0.1);
		ImGui::NextColumn();

		ImGui::Text("Diffuse");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Diffuse", (float*)&conevecs[selection]->Diffuse);
		ImGui::NextColumn();

		ImGui::Text("Specular");
		ImGui::SameLine();
		ImGui::NextColumn();
		ImGui::ColorEdit3("##Specular", (float*)&conevecs[selection]->Specular);
		ImGui::EndChild();
		
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
		fileName = FileExplorerDialog(GameObjVec);
		if (fileName != "") {
			objname = "";
			ImGui::OpenPopup("NewObject1");
		}
	}
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("NewObject1", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("FilePath: %s", fileName.c_str());
		ImGui::InputText("Enter Name ", &objname);
		if (objname == "")
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please Enter a Name");
		}
		if (ImGui::Button("Ok", ImVec2(120, 0)))
		{
			if (objname != "")
			{
				GameObject* newObj = new GameObject(fileName, false, objname);
				GameObjVec.push_back(newObj);
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
		ImGui::EndPopup();
	}

	/*if (ImGui::Button("Delete Selected GameObject")) {
		DeleteObj(selectedGameObj, GameObjVec, PointLightVec, suns, conevecs, scenes);
		selectedGameObj--;
	}*/
	if (ImGui::Button("Add Scene")) {
		Scene* scene1 = new Scene;
		scenes.push_back(scene1);
	}
	if (ImGui::Button("Next Scene")) {
		selScene++;
		if (selScene >= scenes.size()) selScene = 0;
	}
	if (ImGui::Button("Add Point Light")) {
		objname = "";
		ImGui::OpenPopup("NewPoint");
	}
	AddPointPopup(PointLightVec);
	if (ImGui::Button("Add Sun Light")) {
		SunLight* stemp = new SunLight(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), "s");
		suns.push_back(stemp);
	}
	if (ImGui::Button("Add Cone Light")) {
		ConeLight* ctemp = new ConeLight(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 20.0f, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), "c");
		conevecs.push_back(ctemp);
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
		ImGuiSunLight(suns, selectedGameObj - currsize);
	}
	currsize += suns.size();
	if (selectedGameObj >= currsize && selectedGameObj < currsize + conevecs.size()) {
		ImGuiConeLight(conevecs, selectedGameObj - currsize);
	}

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
	//Scene* scene0 = new Scene;
	//Scene* scene1 = new Scene;

	//GameObject* bag = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	//GameObject* rock = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
	//GameObject* skull = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");
	//GameObject* bag1 = new GameObject("Models/bag_model/backpack.obj", true, "bag");
	//GameObject* rock1 = new GameObject("Models/basecharacter/funnyrock.obj", false, "rock");
	//GameObject* skull1 = new GameObject("Models/basecharacter/brideskull.obj", false, "skull");

	//scene0->addGameObject(bag, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	//scene0->addGameObject(rock, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	//scene0->addGameObject(skull, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);

	//scene1->addGameObject(bag1, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	//scene1->addGameObject(rock1, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);
	//scene1->addGameObject(skull1, glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f);

	//sceneManager.addScene(scene0);
	//sceneManager.addScene(scene1);

	float ambience = 0.0f;  //define the ambient light strength
	glm::vec3 ambient_light(1.0f, 1.0f, 1.0f);  //ambient light color
	ambient_light *= ambience;

	emissiveShader.Activate();

	GLuint modelID_2 = glGetUniformLocation(emissiveShader.ID, "model");
	GLuint transformID_2 = glGetUniformLocation(emissiveShader.ID, "cameraMatrix");

	diffuseShader.Activate();
	GLuint modelID = glGetUniformLocation(diffuseShader.ID, "model");     //grab the model and the camera transform matrices
	GLuint transformID = glGetUniformLocation(diffuseShader.ID, "cameraMatrix");

	Camera scenecam(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, (float)(width) / height, 0.1f, 100.0f);  //creating the camera
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);

	//making the lights
	//PointLight point(glm::vec3(5.0f, 1.0f, 5.0f), glm::vec3(1.0f), glm::vec3(1.0f), "p1");
	//vector<PointLight*> pointLights = {};
	//pointLights.push_back(&point);

	//SunLight sun(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "s1");
	//vector<SunLight*> sunLights = {};
	//sunLights.push_back(&sun);

	//ConeLight cone(glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 20.0f, glm::vec3(1.0f, 1.0f, 0.8f), glm::vec3(1.0f, 1.0f, 0.8f), "c1");
	//vector<ConeLight*> coneLights = {};
	//coneLights.push_back(&cone);

	//sceneManager.scenes[0]->points = pointLights;
	//sceneManager.scenes[0]->suns = sunLights;
	//sceneManager.scenes[0]->cones = coneLights;

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

		//RenderLights(emissiveShader, pointLights, suns, coneslights);

		diffuseShader.Activate();

		diffuseShader.Setvec3("light.position", light_pos);
		diffuseShader.Setvec3("light.diffuse", light_color);
		diffuseShader.Setvec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		diffuseShader.Setvec3("material.diffuse", glm::vec3(1.0f, 0.5f, 0.31f));
		diffuseShader.Setvec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
		diffuseShader.Set1f("material.shine", 32.0f);

		PassPointsToShader(diffuseShader, sceneManager.scenes[selScene]->points);
		PassSunsToShader(diffuseShader, sceneManager.scenes[selScene]->suns);
		PassConesToShader(diffuseShader, sceneManager.scenes[selScene]->cones);

		GLuint lightID = glGetUniformLocation(diffuseShader.ID, "light_color");
		GLuint colorID = glGetUniformLocation(diffuseShader.ID, "mycolor");

		diffuseShader.Setvec3("ambience", ambient_light);
		diffuseShader.Setvec3("cameraPos", scenecam.Position);
		diffuseShader.Setvec3("lightColor", light_color);
		diffuseShader.Setvec3("lightpos", light_pos);
		diffuseShader.Set1f("tmaterial.shine", 64.0f);

		glUniformMatrix4fv(transformID, 1, GL_FALSE, glm::value_ptr(transform));

		// Rendering Current Scene
		sceneManager.switchToScene(selScene);
		sceneManager.renderCurrentScene(diffuseShader, emissiveShader);

		if (!focus)
		{
			Gui(sceneManager.scenes[selScene]->gameObjects, sceneManager.scenes[selScene]->points, sceneManager.scenes[selScene]->suns, sceneManager.scenes[selScene]->cones, sceneManager.scenes);
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
	/*GameObject* selectedGO = sceneManager.scenes[sceneManager.currentSceneIndex]->gameObjects[SelObj];
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
	}*/
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
		if (focus==true)
		{
			cout << "focus on"<<endl;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, scenecam.xposMouse, scenecam.yposMouse);
		}
		else {
			cout << "focus off"<<endl;
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			int width, height;
			glfwGetWindowSize(window,&width, &height);
			glfwSetCursorPos(window, width/2, height/2);
			//glfwSetCursorPos(window, 1920 / 2, 1080 / 2);
		}
		cout << focus;
	}
}