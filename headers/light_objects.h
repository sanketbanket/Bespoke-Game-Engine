#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <glm/vec3.hpp>
#include "shaderClass.h"
#include <string>
#include <vector>
#include "Model.h"



class Light {
public:
	float strength = 1.0f;
	string name;
	virtual ~Light() {};
};

class PointLight : public Light {
public:
	string type = "point";
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	Model* display = new Model("models/Lights/PointLight.obj");
	PointLight() {}
	PointLight(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec, string na) {
		Position = pos;
		Diffuse = glm::normalize(diff);
		Specular = glm::normalize(spec);
		name = na;
	};
	void Render(Shader& shader) {
		shader.Activate();
		glm::mat4 model(1.0f);
		model = glm::translate(model, Position);
		model = glm::scale(model, glm::vec3(0.2f));
		shader.Setmat4("model", model);
		display->Draw(shader);
	}

	std::vector<float> get_location() {
		std::vector<float> return_pose{};
		for (int i{}; i < 3; i++) {
			return_pose.push_back(this->Position[i]);
		}
		for (int i{ 3 }; i < 6; i++) {
			return_pose.push_back(this->Diffuse[i - 3]);
		}
		for (int i{ 6 }; i < 9; i++) {
			return_pose.push_back(this->Specular[i - 6]);
		}
		return_pose.push_back(this->strength);
		return return_pose;
	}
	void set_location(std::vector<float>& input_pose) {
		for (int i{}; i < 3; i++) {
			this->Position[i] = input_pose[i];
		}
		for (int i{ 3 }; i < 6; i++) {
			this->Diffuse[i - 3] = input_pose[i];
		}
		for (int i{ 6 }; i < 9; i++) {
			this->Specular[i - 6] = input_pose[i];
		}
		this->strength = input_pose[9];
		return;
	}
};
class SunLight : public Light {
public:
	string type = "sun";
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	Model* display = new Model("models/Lights/SunLight.obj");

	SunLight() {}
	SunLight(glm::vec3 dir, glm::vec3 diff, glm::vec3 spec, string na) {
		Direction = dir;
		Diffuse = glm::normalize(diff);
		Specular = glm::normalize(spec);
		name = na;
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
	std::vector<float> get_location() {
		std::vector<float> return_pose{};
		for (int i{}; i < 3; i++) {
			return_pose.push_back(this->Position[i]);
		}
		for (int i{ 3 }; i < 6; i++) {
			return_pose.push_back(this->Direction[i - 3]);
		}
		for (int i{ 6 }; i < 9; i++) {
			return_pose.push_back(this->Diffuse[i - 6]);
		}
		for (int i{ 9 }; i < 12; i++) {
			return_pose.push_back(this->Specular[i - 9]);
		}
		return_pose.push_back(this->strength);
		return return_pose;
	}
	void set_location(std::vector<float>& input_pose) {
		for (int i{}; i < 3; i++) {
			this->Position[i] = input_pose[i];
		}
		for (int i{3}; i < 6; i++) {
			this->Direction[i - 3] = input_pose[i];
		}
		for (int i{ 6 }; i < 9; i++) {
			this->Diffuse[i - 6] = input_pose[i];
		}
		for (int i{ 9 }; i < 12; i++) {
			this->Specular[i - 9] = input_pose[i];
		}
		this->strength = input_pose[12];
		return;
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

	ConeLight() {}
	ConeLight(glm::vec3 pos, glm::vec3 dir, float cutoff, glm::vec3 diff, glm::vec3 spec, string na) {
		Direction = dir;
		Diffuse = glm::normalize(diff) ;
		Specular = glm::normalize(spec);
		Position = pos;
		Cutoff = cutoff;
		name = na;
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
	std::vector<float> get_location() {
		std::vector<float> return_pose{};
		for (int i{}; i < 3; i++) {
			return_pose.push_back(this->Position[i]);
		}
		for (int i{ 3 }; i < 6; i++) {
			return_pose.push_back(this->Direction[i - 3]);
		}
		for (int i{ 6 }; i < 9; i++) {
			return_pose.push_back(this->Diffuse[i - 6]);
		}
		for (int i{ 9 }; i < 12; i++) {
			return_pose.push_back(this->Specular[i - 9]);
		}
		return_pose.push_back(this->Cutoff);
		return_pose.push_back(this->strength);
		return return_pose;
	}
	void set_location(std::vector<float>& input_pose) {
		for (int i{}; i < 3; i++) {
			this->Position[i] = input_pose[i];
		}
		for (int i{ 3 }; i < 6; i++) {
			this->Direction[i - 3] = input_pose[i];
		}
		for (int i{ 6 }; i < 9; i++) {
			this->Diffuse[i - 6] = input_pose[i];
		}
		for (int i{ 9 }; i < 12; i++) {
			this->Specular[i - 9] = input_pose[i];
		}
		this->Cutoff = input_pose[12];
		this->strength = input_pose[13];
		return;
	}
};


void ApplyPointToShader(Shader& shader, PointLight* light, int index) {
	string name = "Points[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
}
void ApplySunToShader(Shader& shader, SunLight* light, int index) {
	string name = "Suns[";
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
}
void ApplyConeToShader(Shader& shader, ConeLight* light, int index) {
	string name = "Cones[";
	shader.Setvec3((name + to_string(index) + "].position").c_str(), light->Position);
	shader.Setvec3((name + to_string(index) + "].direction").c_str(), light->Direction);
	shader.Setvec3((name + to_string(index) + "].diffuse").c_str(), light->Diffuse * light->strength);
	shader.Setvec3((name + to_string(index) + "].specular").c_str(), light->Specular * light->strength);
	shader.Set1f((name + to_string(index) + "].cutoff_angle").c_str(), light->Cutoff);
}

void PassPointsToShader(Shader& shader, vector<PointLight*> lights) {
	shader.Set1i("pointCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyPointToShader(shader, lights[i], i);
	}
}
void PassSunsToShader(Shader& shader, vector<SunLight*> lights) {
	shader.Set1i("sunCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplySunToShader(shader, lights[i], i);
	}
}
void PassConesToShader(Shader& shader, vector<ConeLight*> lights) {
	shader.Set1i("coneCount", lights.size());
	if (lights.empty()) {
		return;
	}
	for (int i = 0; i < lights.size(); i++) {
		ApplyConeToShader(shader, lights[i], i);
	}
}

void RenderLights(Shader& shader, vector<PointLight*> points = {}, vector<SunLight*> suns = {}, vector<ConeLight*> cones = {}) {
	if (!suns.empty()) {
		for (int i = 0; i < suns.size(); i++) {
			suns[i]->Render(shader);
		}
	}
	if (!cones.empty()) {
		for (int i = 0; i < cones.size(); i++) {
			cones[i]->Render(shader);
		}
	}
	if (!points.empty()) {
		for (int i = 0; i < points.size(); i++) {
			points[i]->Render(shader);
		}
	}
}


#endif
