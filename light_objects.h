#ifndef LIGHT_CLASS_H
#define LIGHT_CLASS_H

#include <glm/vec3.hpp>
class PointLight {
public:
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	PointLight(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec) {
		Position = pos;
		Diffuse = diff;
		Specular = spec;
	};
};
class SunLight {
public:
	glm::vec3 Direction = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	SunLight(glm::vec3 dir, glm::vec3 diff, glm::vec3 spec) {
		Direction = dir;
		Diffuse = diff;
		Specular = spec;
	};
};
class ConeLight {
public:
	glm::vec3 Position = glm::vec3(0.0f);
	glm::vec3 Diffuse = glm::vec3(0.0f);
	glm::vec3 Specular = glm::vec3(0.0f);
	float Cutoff;
	ConeLight(glm::vec3 pos, glm::vec3 diff, glm::vec3 spec, float cutoffdeg) {
		Position = pos;
		Diffuse = diff;
		Specular = spec;
		Cutoff = cutoffdeg;
	};
};

#endif
