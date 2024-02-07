#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <glm/vec3.hpp>
#include "shaderClass.h"
#include <string>
#include <vector>
#include "Model.h"

class Light {
public : 
	virtual ~Light() {};
};

class PointLight : public Light {
public:
	string type = "point";
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	Model* display= new Model("models/Lights/PointLight.obj");
	PointLight(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec) {
		Position = pos;
		Diffuse = diff;
		Specular = spec;
	};
	void Render(Shader& shader) {
		shader.Activate();
		glm::mat4 model(1.0f);
		model = glm::translate(model, Position);
		model = glm::scale(model, glm::vec3(0.2f));
		shader.Setmat4("model", model);
		display->Draw(shader);
	}
};
class SunLight : public Light{
public:
	string type = "sun";
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Direction = glm::vec3(0.0f,-1.0f, 0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	Model* display = new Model("models/Lights/SunLight.obj");

	SunLight(glm::vec3 dir, glm::vec3 diff, glm::vec3 spec) {
		Direction = dir;
		Diffuse = diff;
		Specular = spec;
	};
	void Render(Shader& shader) {
		shader.Activate();
		Direction = glm::normalize(Direction);
		glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 axis = -glm::normalize(glm::cross(Direction, down));
		float angle = acos(glm::dot(Direction, down));
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::rotate(model, angle, axis);

		model = glm::scale(model, glm::vec3(0.2f));

		
		shader.Setmat4("model", model);
		display->Draw(shader);
	}
};
class ConeLight : public Light {
public:
	string type = "cone";
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	glm::vec3 Direction = glm::vec3(0.0f);
	float Cutoff;
	Model* display = new Model("models/Lights/ConeLight.obj");
	ConeLight(glm::vec3 pos, glm::vec3 dir, float cutoff, glm::vec3 diff, glm::vec3 spec) {
		Direction = dir;
		Diffuse = diff;
		Specular = spec;
		Position = pos;
		Cutoff = cutoff;
	};

	void Render(Shader& shader) {
		shader.Activate();
		Direction = glm::normalize(Direction);
		glm::vec3 down = glm::vec3(0.0f, -1.0f, 0.0f);
		glm::vec3 axis = -glm::normalize(glm::cross(Direction, down));
		float angle = acos(glm::dot(Direction, down));
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, Position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, glm::vec3(0.2f));


		shader.Setmat4("model", model);
		display->Draw(shader);
	}
}


void ApplyPointToShader(Shader& shader, PointLight *light, int index) {
	string name = "Points[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular);
}
void ApplySunToShader(Shader& shader, SunLight *light, int index) {
	string name = "Suns[";
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular);
}
void ApplyConeToShader(Shader& shader, ConeLight *light, int index) {
	string name = "Cones[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular);
	shader.Set1f((name + to_string(index) + "].cutoff_angle").c_str(), light->Cutoff);
}
 
void PassPointsToShader(Shader &shader, vector<PointLight*> lights){
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyPointToShader(shader, lights[i], i);
	}
	shader.Set1i("pointCount", lights.size());
}
void PassSunsToShader(Shader& shader, vector<SunLight*> lights) {
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplySunToShader(shader, lights[i], i);
	}
	shader.Set1i("sunCount", lights.size());
}
void PassConesToShader(Shader& shader, vector<ConeLight*> lights) {
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyConeToShader(shader, lights[i], i);
	}
	shader.Set1i("coneCount", lights.size());
}

#endif
