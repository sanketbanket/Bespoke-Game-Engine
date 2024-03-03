#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>
#include <memory>
#include "headers/GameObj.h"
#include "headers/light_objects.h"

class Scene {
public:
    // Add a game object to the scene
    void addGameObject(GameObject* object, glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), float xaxisanglem = 0.0f, float yaxisanglem = 0.0f, float zaxisanglem = 0.0f, float scale = 1.0f) {
        object->tvecm = position;
        object->xaxisanglem = xaxisanglem;
        object->yaxisanglem = yaxisanglem;
        object->zaxisanglem = zaxisanglem;
        gameObjects.push_back(object);
    }

    // Add a light object to the scene
    void addPointLight(PointLight* point, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f), std::string nam = "na") {
        point->Position = position;
        point->Diffuse = diff;
        point->Specular = spec;
        point->name = nam;
        points.push_back(point);
    }

    void addSunLight(SunLight* sun, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 direc = glm::vec3(0.0f, 0.0f, 1.0f), std::string nam = "na") {
        sun->Position = position;
        sun->Direction = direc;
        sun->Diffuse = diff;
        sun->Specular = spec;
        sun->name = nam;
        suns.push_back(sun);
    }

    void addConeLight(ConeLight* cone, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 diff = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 spec = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 direc = glm::vec3(0.0f, 0.0f, 1.0f), float cutoff = 45.0f, std::string nam = "na") {
        cone->Position = position;
        cone->Direction = direc;
        cone->Diffuse = diff;
        cone->Specular = spec;
        cone->Cutoff = cutoff;
        cone->name = nam;
        cones.push_back(cone);
    }

    // Render the scene
    void render(Shader objectShader, Shader lightShader) {
        for (GameObject* object : gameObjects) {
            // Apply transform and render the game object
            object->draw(objectShader);
        }
        RenderLights(lightShader, points, suns, cones);
    }
    vector<GameObject*> gameObjects;
    vector<PointLight*> points;
    vector<SunLight*> suns;
    vector<ConeLight*> cones;
};

class SceneManager {
public:
    // Add a scene to the scene manager
    void addScene(Scene* scene) {
        scenes.push_back(scene);
    }

    // Switch to a scene by index
    void switchToScene(size_t index) {
        if (index < scenes.size()) {
            currentSceneIndex = index;
        }
        else {
            std::cerr << "Invalid scene index" << std::endl;
        }
    }

    // Render the current scene
    void renderCurrentScene(Shader objectShader, Shader lightShader) {
        if (currentSceneIndex < scenes.size()) {
            scenes[currentSceneIndex]->render(objectShader, lightShader);
        }
        else {
            std::cerr << "No scene available" << std::endl;
        }
    }

    size_t currentSceneIndex = 0;
    vector<Scene*> scenes;
};



#endif
