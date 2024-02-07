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
	float scalem = 1.0f;
	glm::vec3 svecm = glm::vec3(1.0f, 1.0f, 1.0f);
	float xaxisanglem = 0.0f;
	float yaxisanglem = 0.0f;
	float zaxisanglem = 0.0f;
    bool fliptextures = false;

	Model ourModel;
	GameObject()
	{

	}
    GameObject(std::string path) {
        this->path = path;
    }
	GameObject(const std::string& modelpath, bool fliptextures)
	{
        this->fliptextures = fliptextures;
        this->path = modelpath;
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
        this->scalem = input_pose[25];
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
        return_pose.push_back(this->scalem);
        return_pose.push_back(this->fliptextures);
        return return_pose;
    }
};

class saving_loading {

public:
    saving_loading() {}

    void saving(std::vector<GameObject*>& inputObject) {
        std::ofstream file;
        file.open("myFile.txt");
        for (int k{}; k < inputObject.size(); k++) {
            std::string path = inputObject[k]->path;
            std::vector<float> current_mat = inputObject[k]->get_location();
            file << path;
            file << "\n";
            for (int i{}; i < 27; i++) {
                file << current_mat[i];
                file << " ";
            }
            file << "\n";
        }
        file.close();
        return;
    }

    std::vector<GameObject*> loading() {
        std::ifstream file("myFile.txt");
        std::string line{};
        std::vector<GameObject*> ret_vector{};
        if (file.is_open()) {
            while (getline(file, line)) {
                std::string path = line;
                std::vector<float> inputPose{};
                getline(file, line);
                std::istringstream iss(line);
                float floatValue{};
                int i{};
                while (i < 27) {
                    iss >> floatValue;
                    inputPose.push_back(floatValue);
                    i++;
                }
                // gameObject->specifications = arr;
                GameObject* gameObject = new GameObject(path,inputPose[26]);
                gameObject->set_location(inputPose);
                ret_vector.push_back(gameObject);
            }
            file.close();
        }
        else {
            std::cout << "file is not open"
                << "\n";
        }
        return ret_vector;
    }
};

#endif