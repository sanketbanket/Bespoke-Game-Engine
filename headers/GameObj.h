#ifndef GameObjClass
#define GameObjClass

#include "shaderClass.h"
#include "cameraClass.h"
#include "Model.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

class GameObject
{
public: 
	glm::mat4 GOmodelmat = glm::mat4(1.f);
	glm::vec3 tvecm = glm::vec3(0.0f, 0.0f, 0.0f);
	float scalem = 1.0f;
	glm::vec3 svecm = glm::vec3(1.0f, 1.0f, 1.0f);
	float xaxisanglem = 0.0f;
	float yaxisanglem = 0.0f;
	float zaxisanglem = 0.0f;
	Model ourModel;
	GameObject()
	{

	}
	GameObject(const std::string& modelpath, bool fliptextures)
	{
		stbi_set_flip_vertically_on_load(fliptextures);
		ourModel = modelpath;
	}

	void transform(Shader ourShader)
	{
		GOmodelmat = glm::mat4(1.f);
		GOmodelmat = glm::translate(GOmodelmat, tvecm);
		GOmodelmat = glm::scale(GOmodelmat, glm::vec3(scalem,scalem,scalem));
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(xaxisanglem), glm::vec3(1.0f, 0.0f, 0.0f));
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(yaxisanglem), glm::vec3(0.0f, 1.0f, 0.0f));
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(zaxisanglem), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.Setmat4("model", GOmodelmat);
	}
	void draw(PickingTechnique technique) {
		ourModel.Draw(technique);
	}
	void draw(Shader ourShader) 
	{
		//GOmodelmat = glm::mat4(1.f);
		//GOmodelmat = glm::translate(GOmodelmat, glm::vec3(1.0f, 0.0f, 0.0f)); // translate it right
		//GOmodelmat = glm::scale(GOmodelmat, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
		//ourShader.setMat4("model", GOmodelmat);
		ourModel.Draw(ourShader);
	}
};

#endif