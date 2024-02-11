#ifndef SCENE_H
#define SCENE_H

#include <iostream>
#include <vector>
#include <memory>
#include "headers/GameObj.h"

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

    // Render the scene
    void render(Shader ourShader) {
        for (GameObject* object : gameObjects) {
            // Apply transform and render the game object
            object->draw(ourShader);
        }
    }
    vector<GameObject*> gameObjects;
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
    void renderCurrentScene(Shader ourShader) {
        if (currentSceneIndex < scenes.size()) {
            scenes[currentSceneIndex]->render(ourShader);
        }
        else {
            std::cerr << "No scene available" << std::endl;
        }
    }

    size_t currentSceneIndex = 0;
    vector<Scene*> scenes;
};



#endif
