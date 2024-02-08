#ifndef SavingLoading
#define SavingLoading

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include "Scene.h"
#include "GameObj.h"

class saving_loading {

public:
    saving_loading() {}

    void saving(std::vector<Scene*>& inputObject) {
        std::ofstream file;
        file.open("myFile.txt");
        for (int i{}; i < inputObject.size(); i++) {
            int numObjects = inputObject[i]->gameObjects.size();
            file << numObjects;
            file << " ";
        }
        file << "\n";
        for (int k{}; k < inputObject.size(); k++) {
            for (int j{}; j < inputObject[k]->gameObjects.size(); j++) {
                std::string path = inputObject[k]->gameObjects[j]->path;
                std::vector<float> current_mat = inputObject[k]->gameObjects[j]->get_location();
                file << path;
                file << "\n";
                for (int i{}; i < 27; i++) {
                    file << current_mat[i];
                    file << " ";
                }
                file << "\n";
            }
        }
        file.close();
        return;
    }

    std::vector<Scene*> loading() {
        std::ifstream file("myFile.txt");
        std::string line{};
        std::vector<Scene*> scenes{};

        if (file.is_open()) {
            getline(file, line);
            std::istringstream iss(line);
            int intValue{};
            std::vector<int> numObjectsScenes{};
            while (iss >> intValue) {
                numObjectsScenes.push_back(intValue);
            }
            for (int i{}; i < numObjectsScenes.size();i++) {
                Scene* newScene = new Scene();
                std::vector<GameObject*> ret_vector{};
                for (int j{}; j < numObjectsScenes[i];j++) {
                    getline(file, line);
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
                    GameObject* gameObject = new GameObject(path, inputPose[26]);
                    gameObject->set_location(inputPose);
                    ret_vector.push_back(gameObject);
                }
                newScene->gameObjects = ret_vector;
                scenes.push_back(newScene);
            }
            file.close();
        }
        else {
            std::cout << "file is not open"
                << "\n";
        }
        return scenes;
    }
};

#endif