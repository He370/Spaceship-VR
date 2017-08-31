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

class Stones
{
	std::string name;

	float S_scale = 1.0f;
	float S_sperate = 1.0f;
	float S_radius = 1.0f;
	float S_pos_x = 0.0f;
	float S_pos_y = 0.0f;
	float S_pos_z = 0.0f;

public:

	MeshData S_mesh;
	GLuint texture_stone = -1;

	void init(std::string meshName,std::string textureName);
	void setParameters(float scale,float sperate,float radius);
	void setPosition(float x, float y, float z);
	void draw(glm::mat4 PV,glm::mat4 M0, float r_angle);
	void drawDebugUI();

	Stones();
	~Stones();
};

