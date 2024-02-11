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
    std::string path{};
	glm::mat4 GOmodelmat = glm::mat4(1.f);
	glm::vec3 tvecm = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 svecm = glm::vec3(1.0f, 1.0f, 1.0f);
	float xaxisanglem = 0.0f;
	float yaxisanglem = 0.0f;
	float zaxisanglem = 0.0f;
    bool fliptextures = false;
	Model ourModel;
    string name;
	GameObject()
	{

	}
    GameObject(std::string path) {
        this->path = path;
    }
	GameObject(const std::string& modelpath, bool fliptextures, const std::string& objectname)
	{
        this->name = objectname;
        this->fliptextures = fliptextures;
        this->path = modelpath;
		stbi_set_flip_vertically_on_load(fliptextures);
		ourModel = modelpath;
	}

	void transform(Shader ourShader)
	{
		GOmodelmat = glm::mat4(1.f);
		GOmodelmat = glm::translate(GOmodelmat, tvecm);
		GOmodelmat = glm::scale(GOmodelmat, svecm);
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(xaxisanglem), glm::vec3(1.0f, 0.0f, 0.0f));
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(yaxisanglem), glm::vec3(0.0f, 1.0f, 0.0f));
		GOmodelmat = glm::rotate(GOmodelmat, glm::radians(zaxisanglem), glm::vec3(0.0f, 0.0f, 1.0f));
		ourShader.Setmat4("model", GOmodelmat);
	}

	void draw(Shader ourShader) 
	{
		transform(ourShader);
		ourModel.Draw(ourShader);
	}

    void set_location(std::vector<float>& input_pose) {
        for (int i{}; i < 16; i++) {
            GOmodelmat[(int)i / 4][i % 4] = input_pose[i];
        }
        for (int i{ 16 }; i < 19; i++) {
            this->tvecm[i - 16] = input_pose[i];
        }
        for (int i{ 19 }; i < 22; i++) {
            this->svecm[i - 19] = input_pose[i];
        }
        this->xaxisanglem = input_pose[22];
        this->yaxisanglem = input_pose[23];
        this->zaxisanglem = input_pose[24];
        this->fliptextures = input_pose[26];
        return;
    }

    std::vector<float> get_location() {
        std::vector<float> return_pose{};
        for (int i{}; i < 16; i++) {
            return_pose.push_back(this->GOmodelmat[(int)i / 4][i % 4]);
        }
        for (int i{ 16 }; i < 19; i++) {
            return_pose.push_back(this->tvecm[i - 16]);
        }
        for (int i{ 19 }; i < 22; i++) {
            return_pose.push_back(this->svecm[i - 19]);
        }
        return_pose.push_back(this->xaxisanglem);
        return_pose.push_back(this->yaxisanglem);
        return_pose.push_back(this->zaxisanglem);
        return_pose.push_back(this->fliptextures);
        return return_pose;
    }
};

#endif