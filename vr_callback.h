#pragma once

#include <openvr.h>
#include "SDL_keycode.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

extern glm::mat4 M_controller[2];
extern bool b_right_trigger_pressed;

void idle();

void vr_init(vr::IVRSystem* hmd);
void vr_event_callback(vr::VREvent_t* event);
void vr_motion_callback(vr::TrackedDeviceIndex_t device, vr::VRControllerState_t* state, glm::mat4& M);
void vr_keydown(SDL_Keycode key);
