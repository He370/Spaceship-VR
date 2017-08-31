#include <windows.h>

#include <GL/glew.h>

#include <GL/freeglut.h>

#include <GL/gl.h>
#include <GL/glext.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "vr_callback.h"
#include "FinalVR.h"

#include "InitShader.h"
#include "LoadMesh.h"
#include "LoadTexture.h"
#include "imgui_impl_glut.h"
#include "imgui_impl_vr.h"

#include "planet.h"
#include "Stones.h"
#include "Spaceship.h"
#include "Cube.h"

using namespace std;

int width = 1600;
int height = 900;

static const std::string vertex_shader("fbo_vs.glsl");
static const std::string fragment_shader("fbo_fs.glsl");

GLuint shader_program = -1;

MeshData cube_mesh;

// uniform locations
int Ka_loc = -1;
int Kd_loc = -1;
int Ks_loc = -1;
int LightOption_loc = -1;
int m_loc = -1;
int eta_loc = -1;

GLuint quad_vao = -1;
GLuint quad_vbo = -1;

GLuint fbo_id = -1;       // Framebuffer object,
GLuint rbo_id = -1;       // and Renderbuffer (for depth buffering)
GLuint fbo_texture = -1;  // Texture rendered into.

float time_sec = 0.0f;
float angle = 0.0f;

float scale = 1.0f;
float dolly = 1.0f;

int texture_magnification_mode = 1;
int texture_minification_mode = 1;

glm::mat4 PV;

static bool LOD = false;

bool check_framebuffer_status();

Planet planet;
Planet planet2;
Stones stones;
Stones stoneSet2;
Spaceship ship;

// lighting option

static int lightOption = 0;
static float ambient_color = 0.005f;
static float diffuse_color = 0.45f;
static float spec_color = 0.1f;
float L_eta = 1.5;
float L_m = 1.0;

// VR
GLuint vr_fbo;
glm::mat4 PVvr_last;
glm::mat4 Mvr;
glm::mat4 M_translate;
glm::mat4 HMDpose;

glm::mat4 Mr = glm::scale(glm::vec3(1.0f));
glm::mat4 Ml = glm::scale(glm::vec3(1.0f));

glm::vec4 RightController_Direction = glm::vec4(0.0f);
glm::vec3 vr_Center_Position = glm::vec3(0.0f);

float vr_center_distance = 0;

//Parameters
float skybox_scale = 1.74f;

float stone_y = 13.0f;
float stone2_y = 7.1f;
float stone2_radius = 48.0f;
float stone2_scale = 1.0f;

float stone_rotate[10];

//Modes
bool b_spaceship_mode = false;

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam) {

	cout << "---------------------opengl-callback-start------------" << endl;
	cout << "message: " << message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;

	cout << "id: " << id << endl;
	cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
}

void draw_pass_1()
{
	using namespace glm;
	glBindFramebuffer(GL_FRAMEBUFFER, vr_fbo);
	glDisable(GL_CULL_FACE);

   const int pass = 1;

   int pass_loc = glGetUniformLocation(shader_program, "pass");
   if(pass_loc != -1)
   {
      glUniform1i(pass_loc, pass);
   }

   // lighting
   Ka_loc = glGetUniformLocation(shader_program, "Ka");
   Kd_loc = glGetUniformLocation(shader_program, "Kd");
   Ks_loc = glGetUniformLocation(shader_program, "Ks");
   LightOption_loc = glGetUniformLocation(shader_program, "lightOption");
   m_loc = glGetUniformLocation(shader_program, "m");
   eta_loc = glGetUniformLocation(shader_program, "eta");

   float Ka_temp[3] = { ambient_color,ambient_color,ambient_color };
   float Kd_temp[3] = { diffuse_color,diffuse_color,diffuse_color };
   float Ks_temp[3] = { spec_color,spec_color,spec_color };

   glUniform3fv(Ka_loc, 1, Ka_temp);
   glUniform3fv(Kd_loc, 1, Kd_temp);
   glUniform3fv(Ks_loc, 1, Ks_temp);
   glUniform1i(LightOption_loc, lightOption);
   glUniform1f(m_loc, L_m);
   glUniform1f(eta_loc, L_eta);

   planet.draw(PV,Mvr*M_translate);
   planet2.draw(PV, Mvr*M_translate);
   if(b_spaceship_mode)	ship.draw(PV,Mvr, M_controller[1]);

   stones.setPosition(0.0f,stone_y,0.0f);
   stones.setParameters(1.0f, 1.0f, 1.5f);
   stones.draw(PV, Mvr*M_translate,stone_rotate[0]);
   stones.setParameters(1.0f, 1.0f, 1.9f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[1]);
   stones.setParameters(1.0f, 1.0f, 2.1f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[2]);
   stones.setParameters(1.0f, 1.0f, 2.3f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[3]);
   stones.setParameters(1.0f, 1.0f, 2.5f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[4]);
   stones.setParameters(1.0f, 1.0f, 2.7f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[5]);
   stones.setParameters(1.0f, 1.0f, 3.0f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[6]);
   stones.setParameters(1.0f, 1.0f, 3.2f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[7]);
   stones.setParameters(1.0f, 1.0f, 3.5f);
   stones.draw(PV, Mvr*M_translate, stone_rotate[8]);

   stoneSet2.setPosition(0.0f,stone2_y*50.0f,0.0f);
   stoneSet2.setParameters(stone2_scale,1.0f,stone2_radius);
   stoneSet2.draw(PV, Mvr*M_translate, stone_rotate[9]);
}

void draw_pass_2()
{
   const int pass = 2;
   int pass_loc = glGetUniformLocation(shader_program, "pass");
   if(pass_loc != -1)
   {
      glUniform1i(pass_loc, pass);
   }

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, fbo_texture);

   int tex_loc = glGetUniformLocation(shader_program, "texture");
   if(tex_loc != -1)
   {
      glUniform1i(tex_loc, 0); // we bound our texture to texture unit 0
   }

   glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void draw_pass_3(glm::mat4& P, glm::mat4& V, glm::mat4& M0) {
	using namespace glm;
	glBindFramebuffer(GL_FRAMEBUFFER, vr_fbo);
	glDisable(GL_CULL_FACE);

	const int pass = 3;
	int pass_loc = glGetUniformLocation(shader_program, "pass");
	if (pass_loc != -1)
	{
		glUniform1i(pass_loc, pass);
	}

	glm::mat4 Vsky = V;
	Vsky[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 S = glm::scale(glm::vec3(10.0f*skybox_scale));

	int PV_loc = glGetUniformLocation(shader_program, "PV");
	if (PV_loc != -1)
	{
		glm::mat4 PV = P*Vsky;
		glUniformMatrix4fv(PV_loc, 1, false, glm::value_ptr(PV));
	}

	int M_loc = glGetUniformLocation(shader_program, "M");
	if (M_loc != -1)
	{
		glm::mat4 M = M0*S;
		glUniformMatrix4fv(M_loc, 1, false, glm::value_ptr(M));
	}

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader_program);

	DrawCube(cube_mesh);
}

void Draw2dGui()
{
	ImVec2 texsize = ImGui_Impl_VR_GetTextureSize();
	static int tab = 0;
	ImGui_Impl_VR_NewFrame(0);

	ImGui::SetNextWindowSize(texsize);
	ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::Begin("debug", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
   //create a slider to change the angle variables
   ImGui::SliderFloat("View angle", &angle, -3.141592f, +3.141592f);

   ImGui::SliderFloat("scale", &scale, 0.1f, 10.0f);
   ImGui::SliderFloat("dolly", &dolly, 0.1f, 10.0f);
   
   ImGui::Combo("texture magnification", &texture_magnification_mode, "GL_NEAREST\0GL_LINEAR\0\0");   // Combo using values packed in a single constant string (for really quick combo)

   ImGui::Combo("texture minification", &texture_minification_mode, "GL_NEAREST\0GL_LINEAR\0GL_LINEAR_MIPMAP_LINEAR\0\0");   // Combo using values packed in a single constant string (for really quick combo)

   ImGui::Checkbox("Enable mipmap level visualization", &LOD);

   ImGui::End();
   ImGui_Impl_VR_Render(0);

   ImGui_Impl_VR_NewFrame(1);
   ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
   ImGui::SetNextWindowSize(texsize);
   ImGui::Begin("Lighting", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

   //static bool show = false;
   //ImGui::ShowTestWindow(&show);

	   ImGui::SliderFloat("ambient_color", &ambient_color,0.0,1.0f);
	   ImGui::SliderFloat("diffuse_color", &diffuse_color, 0.0, 1.0f);
	   ImGui::SliderFloat("spec_color", &spec_color, 0.0, 1.0f);

	   ImGui::RadioButton("full model", &lightOption, 0); ImGui::SameLine();
	   ImGui::RadioButton("only Fresnel", &lightOption, 1); ImGui::SameLine();
	   ImGui::RadioButton("only Distribution", &lightOption, 2); ImGui::SameLine();
	   ImGui::RadioButton("only Geometric", &lightOption, 3);

	   ImGui::SliderFloat("parameter m", &L_m, 0.001f, 2.0f);
	   ImGui::SliderFloat("parameter eta", &L_eta, 0.001f, 1.0f);

	ImGui::End();
   ImGui_Impl_VR_Render(1);

   ImGui_Impl_VR_NewFrame(2);
   ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
   ImGui::SetNextWindowSize(texsize);
   ImGui::Begin("stones", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

   stones.drawDebugUI();
   ImGui::SliderFloat("stones_all_y",&stone_y,5.0f,20.0f);
   ImGui::SliderFloat("stone2_y", &stone2_y, 5.0f, 20.0f);
   ImGui::SliderFloat("stone2_radius", &stone2_radius, 10.0f, 50.0f);
   ImGui::SliderFloat("stone2_scale", &stone2_scale, 0.5f, 20.0f);

   ImGui::End();
   ImGui_Impl_VR_Render(2);

   ImGui_Impl_VR_NewFrame(3);
   ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
   ImGui::SetNextWindowSize(texsize);
   ImGui::Begin("Debugging", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

   ImGui::SliderFloat("skybox scale",&skybox_scale,1.0f,5.0f);

   
   string center_dis_text = "Center Distance:" + to_string(vr_center_distance);
   ImGui::Text(center_dis_text.c_str());

   ship.drawDebugUI();
   planet.drawDebugUI();

   ImGui::End();
   ImGui_Impl_VR_Render(3);
   //ImGui::Render();
 }


// glut display callback function.
// This function gets called every time the scene gets redisplayed 
void display(glm::mat4& Pvr, glm::mat4& Vvr0, glm::mat4& Mvr0, GLuint fbo)
{
	vr_fbo = fbo;

	RightController_Direction = glm::normalize(M_controller[1] * glm::vec4(0.0f,0.0f,1.0f,0.0f));
	if(b_spaceship_mode) vr_Center_Position += glm::vec3(RightController_Direction)*ship.s_speed;
	vr_center_distance = glm::length(planet.getPosVec3() + vr_Center_Position);

	//if(vr_center_distance<10.0f) vr_Center_Position -= glm::vec3(RightController_Direction)*ship.s_speed;

	glm::mat2 M_scale;
	//glm::mat4 M_translate;

	if (vr_center_distance > 20) {
		M_scale = glm::scale(glm::vec3(20.0f/ vr_center_distance));
		M_translate = glm::translate(vr_Center_Position*20.0f / vr_center_distance);
	}
	else {
		M_scale = glm::scale(glm::vec3(1.0f));
		M_translate = glm::translate(vr_Center_Position);
	}

	Mvr = Mvr0;
	//glm::mat4 Vvr = Vvr0*Mvr0;
	glm::mat4 Vvr = Vvr0;
	PVvr_last = Pvr*Vvr;
	HMDpose = Mvr0;

	//glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 1.0f, dolly), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 P = glm::perspective(3.141592f / 4.0f, float(width)/float(height), 0.1f, 100.0f);

	//PV = P*V;
	//glm::mat4 V = Vvr*M_translate;
	glm::mat4 V = Vvr;
	glm::mat4 P = Pvr;
	PV = P*V;

   glUseProgram(shader_program);

   //glBindFramebuffer(GL_FRAMEBUFFER, fbo_id); // Render to FBO.
   //glDrawBuffer(GL_COLOR_ATTACHMENT0); //Out variable in frag shader will be written to the texture attached to GL_COLOR_ATTACHMENT0.

   int eye_loc = glGetUniformLocation(shader_program, "eye");
   glm::vec4 eye_temp = Vvr0*glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
   glUniform3f(eye_loc, eye_temp.x, eye_temp.y, eye_temp.z);
   //glUniform3f(eye_loc, eye_temp.x, eye_temp.y, eye_temp.z);

   int time_loc = glGetUniformLocation(shader_program, "time");
   glUniform1f(time_loc, time_sec);

   //clear the FBO
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   draw_pass_3(P,V,Mvr); //skybox
   draw_pass_1();
   
   //idle funcs
   ship.idle();
}

/*
void idle()
{
	glutPostRedisplay();

   const int time_ms = glutGet(GLUT_ELAPSED_TIME);
   time_sec = 0.001f*time_ms;

}
*/

void reload_shader()
{

   GLuint new_shader = InitShader(vertex_shader.c_str(), fragment_shader.c_str());

   if(new_shader == -1) // loading failed
   {
      glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
   }
   else
   {
      glClearColor(0.35f, 0.35f, 0.35f, 0.0f);

      if(shader_program != -1)
      {
         glDeleteProgram(shader_program);
      }
      shader_program = new_shader;

	  
      if(planet.mesh.mVao != -1)
      {
         BufferIndexedVerts(planet.mesh);
      }

	  if (planet2.mesh.mVao != -1)
	  {
		  BufferIndexedVerts(planet2.mesh);
	  }

	  if (stones.S_mesh.mVao != -1)
	  {
		  BufferIndexedVerts(stones.S_mesh);
	  }

	  if (stoneSet2.S_mesh.mVao != -1)
	  {
		  BufferIndexedVerts(stoneSet2.S_mesh);
	  }
	  
   }
}

void LoadShader()
{
	if (shader_program != -1)
	{
		glDeleteProgram(shader_program);
	}

	//create and load shaders
	shader_program = InitShader("fbo_vs.glsl", "fbo_fs.glsl");
	if (shader_program == -1) // loading failed
	{
		glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
	}
	else {
		std::cout << "shader loaded!" << endl;
	}
	glUseProgram(shader_program);
}

void printGlInfo()
{
   std::cout << "Vendor: "       << glGetString(GL_VENDOR)                    << std::endl;
   std::cout << "Renderer: "     << glGetString(GL_RENDERER)                  << std::endl;
   std::cout << "Version: "      << glGetString(GL_VERSION)                   << std::endl;
   std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION)  << std::endl;
}

void initOpenGl(int width,int height)
{
	LoadShader();

	const int w = width;
	const int h = height;

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

   //create texture to render pass 1 into

   glGenTextures(1, &fbo_texture);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D_ARRAY, fbo_texture);
   glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA32F, w, h, 10);

   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
   //glBindTexture(GL_TEXTURE_2D, 0);   

   //Create renderbuffer for depth.
   GLuint depthTex;
   glGenTextures(1, &depthTex);
   glBindTexture(GL_TEXTURE_2D_ARRAY, depthTex);

   glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_DEPTH_COMPONENT32, w, h, 10);

   //Create the framebuffer object
   glGenFramebuffers(1, &fbo_id);
   glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_id); // attach depth renderbuffer
   check_framebuffer_status();
   glBindFramebuffer(GL_FRAMEBUFFER, 0);

   //mesh for pass 1

   planet.init("planet.obj", "earthSurface.jpg", "earthLights.jpg");
   planet2.init("planet.obj", "yavin.jpg", "yavinLight.jpg");
   stones.init("rock3.obj", "stone.jpg");
   stoneSet2.init("rock2.obj", "stone.jpg");
   ship.init();

   planet2.setPosition(0.0f,1.0f,0.0f);
   planet2.setScale(1.0f);
   stones.setParameters(1.0f, 1.0f, 1.74f);
   stones.setPosition(0.0f, 10.0f, 0.0f);
   stoneSet2.setParameters(1.0f, 1.0f, 20.0f);
   stoneSet2.setPosition(0.0f, 10.0f, 0.0f);

   for (int i = 0; i < 10; i++) {
	   stone_rotate[i] = 3.1415f*(float(rand() % 100) / 200.0f);
   }

   //mesh for pass 3 (skybox)

   cube_mesh = CreateCube();

   //mesh for pass 2 (full screen quadrilateral)
   glGenVertexArrays(1, &quad_vao);
   glBindVertexArray(quad_vao);

   float vertices[] = { 1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, -1.0f, -1.0f, 0.0f };

   //create vertex buffers for vertex coords
   glGenBuffers(1, &quad_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
   int pos_loc = glGetAttribLocation(shader_program, "pos_attrib");
   if (pos_loc >= 0)
   {
	   glEnableVertexAttribArray(pos_loc);
	   glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, 0, 0);
   }


}

// glut callbacks need to send keyboard and mouse events to imgui
void keyboard(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyCallback(key);

#if _DEBUG
   std::cout << "key : " << key << ", x: " << x << ", y: " << y << std::endl;
#endif

   switch(key)
   {
      case 'r':
      case 'R':
         reload_shader();
      break;
   }
}

void keyboard_up(unsigned char key, int x, int y)
{
   ImGui_ImplGlut_KeyUpCallback(key);
}

void special_up(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialUpCallback(key);
}

void passive(int x, int y)
{
   ImGui_ImplGlut_PassiveMouseMotionCallback(x,y);
}

void special(int key, int x, int y)
{
   ImGui_ImplGlut_SpecialCallback(key);
}

void motion(int x, int y)
{
   ImGui_ImplGlut_MouseMotionCallback(x, y);
}

void mouse(int button, int state, int x, int y)
{
   ImGui_ImplGlut_MouseButtonCallback(button, state);
}

bool check_framebuffer_status() 
{
    GLenum status;
    status = (GLenum) glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            return true;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			printf("Framebuffer incomplete, incomplete attachment\n");
            return false;
        case GL_FRAMEBUFFER_UNSUPPORTED:
			printf("Unsupported framebuffer format\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			printf("Framebuffer incomplete, missing attachment\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			printf("Framebuffer incomplete, missing draw buffer\n");
            return false;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			printf("Framebuffer incomplete, missing read buffer\n");
            return false;
    }
	return false;
}
