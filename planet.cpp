#include "Planet.h"
#include "imgui.h"

extern GLuint shader_program;
extern float angle;
extern float scale;
extern float time_sec;
extern float Mvr;

void Planet::init(std::string mesh_name, std::string texture_name, std::string texture_02_name)
{
	mesh = LoadMesh(mesh_name);
	texture_planet = LoadTexture(texture_name.c_str());
	texture_02_planet = LoadTexture(texture_02_name.c_str());
}

void Planet::draw(glm::mat4 PV, glm::mat4 M0)
{
	//glm::mat4 M = glm::rotate(angle+time_sec*0.1f, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(3.1415926f*0.5f, glm::vec3(-0.5f, 0.0f, -0.5f))*glm::scale(glm::vec3(mesh.mScaleFactor));
	glm::mat4 M = glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::rotate(3.1415926f*0.5f, glm::vec3(-0.5f, 0.0f, -0.5f))*glm::scale(glm::vec3(mesh.mScaleFactor));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_planet);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_02_planet);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glm::mat4 v_scale = glm::scale(glm::vec3(scale));

	int PV_loc = glGetUniformLocation(shader_program, "PV");
	if (PV_loc != -1)
	{
		glUniformMatrix4fv(PV_loc, 1, false, glm::value_ptr(PV));
	}

	int M_loc = glGetUniformLocation(shader_program, "M");
	if (M_loc != -1)
	{
		glm::mat4 M_t = glm::translate(glm::vec3(p_pos_x,p_pos_y,p_pos_z));
		glm::mat4 M_s = glm::scale(glm::vec3(p_scale));
		glm::mat4 M_temp = M0*M_t*M*v_scale*M_s;
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
	glUniform1i(mode_loc, 0);

	glBindVertexArray(mesh.mVao);
	glDrawElements(GL_TRIANGLES, mesh.mNumIndices, GL_UNSIGNED_INT, 0);
}

void Planet::drawDebugUI()
{
	ImGui::SliderFloat("planet pos_x", &p_pos_x, -20.0f, 20.0f);
	ImGui::SliderFloat("planet pos_y", &p_pos_y, -20.0f, 20.0f);
	ImGui::SliderFloat("planet pos_z", &p_pos_z, -20.0f, 20.0f);
	ImGui::SliderFloat("planet scale", &p_scale, -10.0f, 20.0f);
	ImGui::SliderFloat("planet rotate", &p_angle, 0.1f, 10.0f);
}

Planet::Planet()
{
}


Planet::~Planet()
{
}
