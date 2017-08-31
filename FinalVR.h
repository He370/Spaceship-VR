#pragma once

extern bool b_spaceship_mode;
extern glm::mat4 Mvr;

void display(glm::mat4& Pvr, glm::mat4& Vvr, glm::mat4& Mvr, GLuint fbo);
void initOpenGl(int w,int h);
void Draw2dGui();