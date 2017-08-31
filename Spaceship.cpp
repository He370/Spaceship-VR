#include "Spaceship.h"
#include "imgui.h"
#include <algorithm>

extern GLuint shader_program;
extern float scale;
extern bool b_right_trigger_pressed;

void Spaceship::init()
{
	mesh = LoadMesh(this->mesh_name);
	texture_ship = LoadTexture(texture_name.c_str());
}

void Spaceship::idle()
{
	if (b_right_trigger_pressed) {
		s_speed = std::min(s_max_speed,s_speed+s_accelerate);
	}
	else {
		s_speed = std::max(0.0f, s_speed - s_decelerate);
	}
}

void Spaceship::draw(glm::mat4 PV, glm::mat4 M0, glm::mat4 M)
{
	//glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(3.1415926f*0.5f, glm::vec3(-0.5f, 0.0f, -0.5f))*glm::scale(glm::vec3(mesh.mScaleFactor));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ship);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glm::mat4 v_scale = glm::scale(glm::vec3(p_scale));

	int PV_loc = glGetUniformLocation(shader_program, "PV");
	if (PV_loc != -1)
	{
		glUniformMatrix4fv(PV_loc, 1, false, glm::value_ptr(PV));
	}

	int M_loc = glGetUniformLocation(shader_program, "M");
	if (M_loc != -1)
	{
		glm::mat4 M_rotate = glm::rotate(p_rotate, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 M_temp = M0*M*M_rotate*v_scale;
		glUniformMatrix4fv(M_loc, 1, false, glm::value_ptr(M_temp));
	}

	int tex_loc = glGetUniformLocation(shader_program, "texture");
	if (tex_loc != -1)
	{
		glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
	}

	int tex_02_loc = glGetUniformLocation(shader_program, "texture02");
	if (tex_02_loc != -1)
	{
		glUniform1i(tex_02_loc, 1); // we bound our texture to texture unit 0
	}

	int mode_loc = glGetUniformLocation(shader_program, "mode");
	glUniform1i(mode_loc, 2);

	glBindVertexArray(mesh.mVao);
	glDrawElements(GL_TRIANGLES, mesh.mNumIndices, GL_UNSIGNED_INT, 0);
}

void Spaceship::drawDebugUI()
{
	ImGui::SliderFloat("Ship scale",&p_scale,0.01f,10.f);
	std::string speedtext = "Ship Speed:" + std::to_string(s_speed);
	ImGui::Text(speedtext.c_str());
}

Spaceship::Spaceship()
{
}


Spaceship::~Spaceship()
{
}
