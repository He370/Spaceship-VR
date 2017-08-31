#include "vr_callback.h"
#include <GL/freeglut.h>

static vr::IVRSystem*		g_HMD = 0;
static glm::vec2 g_AxisPos[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
static glm::vec2 g_AxisClickedPos[2] = { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) };
//static float last_axis = 0.0f;
static bool axis_dragging[2] = { false, false };

glm::mat4 M_controller[2] = { glm::mat4(1.0f), glm::mat4(1.0f) };
bool b_right_trigger_pressed = false;

extern bool b_spaceship_mode;

void vr_init(vr::IVRSystem* hmd)
{
	g_HMD = hmd;
}

void idle()
{
	static int last_time = 0;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time - last_time;
	last_time = time;
	float delta_seconds = 0.001f*elapsed;

	const float fixed_time_step = 1.0f / 60.0f;
}

void vr_event_callback(vr::VREvent_t* event)
{
	if (g_HMD == 0)
	{
		return;
	}
	const int left_controller = g_HMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	const int right_controller = g_HMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);

	if (event->trackedDeviceIndex == left_controller) //left controller is gui
	{
		const vr::VREvent_Controller_t controller = event->data.controller;
		if (controller.button == vr::k_EButton_SteamVR_Trigger)
		{
			if (event->eventType == vr::VREvent_ButtonTouch)
			{
				if (b_spaceship_mode)
					b_spaceship_mode = false;
				else
					b_spaceship_mode = true;
			}
		}

		if (controller.button == vr::k_EButton_ApplicationMenu)
		{
			//ImGui_Impl_VR_GrabScreenshot();
		}

		if (controller.button == vr::k_EButton_Grip)
		{
			if (event->eventType == vr::VREvent_ButtonPress)
			{

			}
		}

		if (controller.button == vr::k_EButton_Axis0)
		{

			vr::VRControllerState_t state;
			if (g_HMD->GetControllerState(left_controller, &state))
			{
				if (event->eventType == vr::VREvent_ButtonTouch)
				{
					g_AxisPos[0] = glm::vec2(state.rAxis[0].x, state.rAxis[0].y);
					g_AxisClickedPos[0] = g_AxisPos[0];
					axis_dragging[0] = true;
				}
				else if (event->eventType == vr::VREvent_ButtonUntouch)
				{
					axis_dragging[0] = false;
					g_AxisPos[0] = glm::vec2(0.0f, 0.0f);
					g_AxisClickedPos[0] = g_AxisPos[0];
				}
			}
		}
	}


	//convert right touchpad events to glut mouse events for trackball camera
	if (event->trackedDeviceIndex == right_controller) //right controller is mouse
	{
		const vr::VREvent_Controller_t controller = event->data.controller;
		if (controller.button == vr::k_EButton_SteamVR_Trigger)
		{
			if (event->eventType == vr::VREvent_ButtonPress)
			{
				b_right_trigger_pressed = true;
			}
			else {
				b_right_trigger_pressed = false;
			}
		}
		if (controller.button == vr::k_EButton_Grip)
		{
			if (event->eventType == vr::VREvent_ButtonPress)
			{

			}
		}

		if (controller.button == vr::k_EButton_Axis0)
		{

			vr::VRControllerState_t state;
			if (g_HMD->GetControllerState(right_controller, &state))
			{
				if (event->eventType == vr::VREvent_ButtonTouch)
				{
					g_AxisPos[1] = glm::vec2(state.rAxis[0].x, state.rAxis[0].y);
					g_AxisClickedPos[1] = g_AxisPos[1];
					axis_dragging[1] = true;
				}
				else if (event->eventType == vr::VREvent_ButtonUntouch)
				{
					axis_dragging[1] = false;
					g_AxisPos[1] = glm::vec2(0.0f, 0.0f);
					g_AxisClickedPos[1] = g_AxisPos[1];
				}
				else if (event->eventType == vr::VREvent_ButtonPress)
				{

				}

			}
		}
	}
}

void vr_motion_callback(vr::TrackedDeviceIndex_t device, vr::VRControllerState_t* state, glm::mat4& M)
{
	if (g_HMD == 0)
	{
		return;
	}
	const int left_controller = g_HMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
	const int right_controller = g_HMD->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
	if (device == right_controller)
	{
		M_controller[1] = M;
		if (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
		{

			if (axis_dragging[1] == true)
			{

			}

			g_AxisPos[1] = glm::vec2(state->rAxis[0].x, state->rAxis[0].y);
		}
	}
	if (device == left_controller)
	{
		M_controller[0] = M;
		if (state->ulButtonTouched & ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad))
		{
			if (axis_dragging[0] == true)
			{

			}
			g_AxisPos[0] = glm::vec2(state->rAxis[0].x, state->rAxis[0].y);
		}
	}

}

void vr_keydown(SDL_Keycode key) //should really only use this for debugging
{
	if (key == SDLK_r)
	{
		//LoadShader();
	}
}