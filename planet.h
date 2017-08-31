#pragma once
#include <GL/glew.h>

#include <GL/freeglut.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "LoadMesh.h"
#include "LoadTexture.h"

using namespace std;

class Planet
{
	//const std::string mesh_name = "planet.obj";
	//const std::string texture_name = "earthSurface.jpg";
	//const std::string texture_02_name = "earthLights.jpg";
	//const std::string texture_name = "yavin.jpg";

	//parameters
	float p_pos_x = 4.242f;
	float p_pos_y = 4.398f;
	float p_pos_z = -18.0f;
	float p_scale = 19.0f;
	float p_angle = 0;

public:

	MeshData mesh;
	GLuint texture_planet = -1;
	GLuint texture_02_planet = -1;

	void init(std::string mesh_name, std::string texture_name, std::string texture_02_name);
	void setScale(float scale) { p_scale = scale; }
	void setPosition(float x, float y, float z) { p_pos_x = x; p_pos_y = y; p_pos_z = z; };
	void draw(glm::mat4 PV, glm::mat4 M0);
	void drawDebugUI();
	glm::vec3 getPosVec3() { return glm::vec3(p_pos_x, p_pos_y, p_pos_z); }

	Planet();
	~Planet();
};

