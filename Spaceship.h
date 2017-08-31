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

class Spaceship
{
	const std::string mesh_name = "ship.obj";
	const std::string texture_name = "earthSurface.jpg";

	float s_accelerate = 0.00001f;
	float s_decelerate = 0.00010f;
	float s_max_speed = 0.01f;

public:

	float s_speed = 0.0f;

	float p_scale = 0.05f;
	float p_rotate = 3.1415f;

	MeshData mesh;
	GLuint texture_ship = -1;

	void init();
	void idle();
	void draw(glm::mat4 PV, glm::mat4 M0,glm::mat4 M);
	void drawDebugUI();

	Spaceship();
	~Spaceship();
};

