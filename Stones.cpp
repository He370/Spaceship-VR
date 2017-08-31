#include "Stones.h"
#include "imgui_impl_glut.h"

extern GLuint shader_program;
extern float angle;
extern float scale;

void Stones::init(std::string meshName, std::string textureName)
{
	//S_mesh = LoadMesh(this->S_mesh_name,true);
	//texture_stone = LoadTexture(S_texture_name.c_str());
	name = S_scale;
	S_mesh = LoadMesh(meshName, true);
	texture_stone = LoadTexture(textureName.c_str());
}

void Stones::setParameters(float scale, float sperate, float radius)
{
	S_scale = scale;
	S_sperate = sperate;
	S_radius = radius;
}

void Stones::setPosition(float x, float y, float z)
{
	S_pos_x = x;
	S_pos_y = y;
	S_pos_z = z;
}

void Stones::draw(glm::mat4 PV,glm::mat4 M0,float r_angle)
{
	glm::mat4 M = glm::rotate(angle+ r_angle, glm::vec3(0.0f, 1.0f, 0.0f))*glm::scale(glm::vec3(S_mesh.mScaleFactor));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_stone);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glm::mat4 v_scale = glm::scale(glm::vec3(scale/5.0f));

	int PV_loc = glGetUniformLocation(shader_program, "PV");
	if (PV_loc != -1)
	{
		glUniformMatrix4fv(PV_loc, 1, false, glm::value_ptr(PV));
	}

	int M_loc = glGetUniformLocation(shader_program, "M");
	if (M_loc != -1)
	{
		glm::mat4 M_temp = M0*M*v_scale;
		glUniformMatrix4fv(M_loc, 1, false, glm::value_ptr(M_temp));
	}

	int tex_loc = glGetUniformLocation(shader_program, "texture");
	if (tex_loc != -1)
	{
		glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
	}

	int mode_loc = glGetUniformLocation(shader_program, "mode");
	glUniform1i(mode_loc, 1);

	int x_loc = glGetUniformLocation(shader_program, "S_pos_x");
	int y_loc = glGetUniformLocation(shader_program, "S_pos_y");
	int z_loc = glGetUniformLocation(shader_program, "S_pos_z");
	int radius_loc = glGetUniformLocation(shader_program, "S_radius");
	int scale_loc = glGetUniformLocation(shader_program, "S_scale");

	glUniform1f(x_loc, S_pos_x);
	glUniform1f(y_loc, S_pos_y);
	glUniform1f(z_loc, S_pos_z);
	glUniform1f(radius_loc, S_radius);
	glUniform1f(scale_loc, S_scale);

	glBindVertexArray(this->S_mesh.mVao);
	glDrawElementsInstanced(GL_TRIANGLES, this->S_mesh.mNumIndices, GL_UNSIGNED_INT, 0,1000);
}

void Stones::drawDebugUI()
{
	if (ImGui::CollapsingHeader("stones"))
	{
		ImGui::SliderFloat("S_scale", &S_scale, 0.1f, 10.0f);
		ImGui::SliderFloat("S_sperate", &S_sperate, 0.1f, 10.0f);
		ImGui::SliderFloat("S_pos_x", &S_pos_x, -20.0f, 20.0f);
		ImGui::SliderFloat("S_pos_y", &S_pos_y, -20.0f, 20.0f);
		ImGui::SliderFloat("S_pos_z", &S_pos_z, -20.0f, 20.0f);
		ImGui::SliderFloat("S_radius", &S_radius, 0.1f, 10.0f);
	}
}

Stones::Stones()
{
}


Stones::~Stones()
{
}
