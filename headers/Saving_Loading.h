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
#include "light_objects.h"

class saving_loading {

public:
    saving_loading() {}

    void saving(std::vector<Scene*>& sceneVector) {
        std::ofstream file;
        file.open("myFile.txt");
        file << sceneVector.size();
        file << "\n";
        for (int i{}; i < sceneVector.size(); i++) {
            int numObjects = sceneVector[i]->gameObjects.size();
            int numPoints = sceneVector[i]->points.size();
            int numSuns = sceneVector[i]->suns.size();
            int numCones = sceneVector[i]->cones.size();
            file << numObjects;
            file << " ";
            file << numPoints;
            file << " ";
            file << numSuns;
            file << " ";
            file << numCones;
            file << "\n";
        }
        for (int k{}; k < sceneVector.size(); k++) {
            for (int j{}; j < sceneVector[k]->gameObjects.size(); j++) {
                std::string objname = sceneVector[k]->gameObjects[j]->name;
                std::string path = sceneVector[k]->gameObjects[j]->path;
                std::vector<float> current_mat = sceneVector[k]->gameObjects[j]->get_location();
                file << objname;
                file << "\n";
                file << path;
                file << "\n";
                for (int i{}; i < 26; i++) {
                    file << current_mat[i];
                    file << " ";
                }
                file << "\n";
            }
            for (int j{}; j < sceneVector[k]->points.size(); j++) {
                std::string objname = sceneVector[k]->points[j]->name;
                std::vector<float> current_mat = sceneVector[k]->points[j]->get_location();
                file << objname;
                file << "\n";
                for (int i{}; i < 10; i++) {
                    file << current_mat[i];
                    file << " ";
                }
                file << "\n";
            }
            for (int j{}; j < sceneVector[k]->suns.size(); j++) {
                std::string objname = sceneVector[k]->suns[j]->name;
                std::vector<float> current_mat = sceneVector[k]->suns[j]->get_location();
                file << objname;
                file << "\n";
                for (int i{}; i < 13; i++) {
                    file << current_mat[i];
                    file << " ";
                }
                file << "\n";
            }
            for (int j{}; j < sceneVector[k]->cones.size(); j++) {
                std::string objname = sceneVector[k]->cones[j]->name;
                std::vector<float> current_mat = sceneVector[k]->cones[j]->get_location();
                file << objname;
                file << "\n";
                for (int i{}; i < 14; i++) {
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
            //Getting number of scenes
            getline(file, line);
            std::istringstream iss(line);
            int numScenes{};
            iss >> numScenes;
            std::vector<std::vector<int>> numSceneMatrix{};
            //Getting matrix of gameobjects
            for (int i{}; i < numScenes; i++) {
                getline(file, line);
                std::istringstream iss(line);
                int intValue{};
                std::vector<int> numObjectsScenes{};
                while (iss >> intValue) {
                    numObjectsScenes.push_back(intValue);
                }
                numSceneMatrix.push_back(numObjectsScenes);
            }
            //Adding objects to scene
            for (int i{}; i < numScenes;i++) {
                Scene* newScene = new Scene();
                //GameObjects
                std::vector<GameObject*> gObjects{};
                for (int j{}; j < numSceneMatrix[i][0];j++) {
                    getline(file, line);
                    std::string name = line;
                    getline(file, line);
                    std::string path = line;
                    std::vector<float> inputPose{};
                    getline(file, line);
                    std::istringstream iss(line);
                    float floatValue{};
                    int k{};
                    while (k < 26) {
                        iss >> floatValue;
                        inputPose.push_back(floatValue);
                        k++;
                    }
                    GameObject* gameObject = new GameObject(path, inputPose[25], name);
                    gameObject->set_location(inputPose);
                    gObjects.push_back(gameObject);
                }
                //PointLights
                std::vector<PointLight*> points{};
                for (int j{}; j < numSceneMatrix[i][1];j++) {
                    getline(file, line);
                    std::string name = line;
                    std::vector<float> inputPose{};
                    getline(file, line);
                    std::istringstream iss(line);
                    float floatValue{};
                    int k{};
                    while (k < 10) {
                        iss >> floatValue;
                        inputPose.push_back(floatValue);
                        k++;
                    }
                    PointLight* point = new PointLight();
                    point->name = name;
                    point->set_location(inputPose);
                    points.push_back(point);
                }
                //SunLights
                std::vector<SunLight*> suns{};
                for (int j{}; j < numSceneMatrix[i][2];j++) {
                    getline(file, line);
                    std::string name = line;
                    std::vector<float> inputPose{};
                    getline(file, line);
                    std::istringstream iss(line);
                    float floatValue{};
                    int k{};
                    while (k < 13) {
                        iss >> floatValue;
                        inputPose.push_back(floatValue);
                        k++;
                    }
                    SunLight* sun = new SunLight();
                    sun->name = name;
                    sun->set_location(inputPose);
                    suns.push_back(sun);
                }
                //ConeLights
                std::vector<ConeLight*> cones{};
                for (int j{}; j < numSceneMatrix[i][3];j++) {
                    getline(file, line);
                    std::string name = line;
                    std::vector<float> inputPose{};
                    getline(file, line);
                    std::istringstream iss(line);
                    float floatValue{};
                    int k{};
                    while (k < 14) {
                        iss >> floatValue;
                        inputPose.push_back(floatValue);
                        k++;
                    }
                    ConeLight* cone = new ConeLight();
                    cone->name = name;
                    cone->set_location(inputPose);
                    cones.push_back(cone);
                }
                newScene->gameObjects = gObjects;
                newScene->points = points;
                newScene->suns = suns;
                newScene->cones = cones;
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