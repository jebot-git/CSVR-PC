/*
host_vr.c - virtual reality integration
Copyright (C) 2025 Team Beef

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include <stdbool.h>
#include <SDL.h>
#include <VrBase.h>
#include <VrInput.h>
#include <VrHaptics.h>
#include <VrRenderer.h>

#include "common.h"
#include "client.h"
#include "input.h"
#include <cl_tent.h>
#include <platform/sdl/events.h>
#include "xr_ui.h"
#include "xr_menu.h"

CVAR_DEFINE_AUTO( vr_enabled, "0", FCVAR_READ_ONLY, "An OpenXR session is enabled" );
CVAR_DEFINE_AUTO( vr_camera_x, "0", FCVAR_MOVEVARS, "Offset x of the camera" );
CVAR_DEFINE_AUTO( vr_camera_y, "0", FCVAR_MOVEVARS, "Offset y of the camera" );
CVAR_DEFINE_AUTO( vr_camera_z, "0", FCVAR_MOVEVARS, "Offset z of the camera" );
CVAR_DEFINE_AUTO( vr_force2d, "0", FCVAR_MOVEVARS, "Is the client forcing 2D UI mode?" );
CVAR_DEFINE_AUTO( vr_gamemode, "0", FCVAR_MOVEVARS, "Are we in the 3D VR mode?" );
CVAR_DEFINE_AUTO( vr_hand_active, "0", FCVAR_MOVEVARS, "Hand action active" );
CVAR_DEFINE_AUTO( vr_hand_click, "0", FCVAR_MOVEVARS, "Hand aiming click" );
CVAR_DEFINE_AUTO( vr_hand_stretch, "0", FCVAR_MOVEVARS, "Hand aiming with arm stretch" );
CVAR_DEFINE_AUTO( vr_hand_x, "0", FCVAR_MOVEVARS, "Hand position x" );
CVAR_DEFINE_AUTO( vr_hand_y, "0", FCVAR_MOVEVARS, "Hand position y" );
CVAR_DEFINE_AUTO( vr_hand_z, "0", FCVAR_MOVEVARS, "Hand position z" );
CVAR_DEFINE_AUTO( vr_hand_pitch, "0", FCVAR_MOVEVARS, "Hand pitch angle" );
CVAR_DEFINE_AUTO( vr_hand_yaw, "0", FCVAR_MOVEVARS, "Hand yaw angle" );
CVAR_DEFINE_AUTO( vr_hand_roll, "0", FCVAR_MOVEVARS, "Hand roll angle" );
CVAR_DEFINE_AUTO( vr_hand_swap, "0", FCVAR_MOVEVARS, "Hand/weapon swap during dual hand weapons" );
CVAR_DEFINE_AUTO( vr_haptics_weapon, "0", FCVAR_MOVEVARS, "Haptics amount for the weapon" );
CVAR_DEFINE_AUTO( vr_haptics_yaw, "0", FCVAR_ARCHIVE, "Direction from which the attack came" );
CVAR_DEFINE_AUTO( vr_hmd_pitch, "0", FCVAR_MOVEVARS, "Camera pitch angle" );
CVAR_DEFINE_AUTO( vr_hmd_yaw, "0", FCVAR_MOVEVARS, "Camera yaw angle" );
CVAR_DEFINE_AUTO( vr_hmd_roll, "0", FCVAR_MOVEVARS, "Camera roll angle" );
CVAR_DEFINE_AUTO( vr_ladder, "0", FCVAR_MOVEVARS, "Flag if player is on ladder" );
CVAR_DEFINE_AUTO( vr_offset_x, "0", FCVAR_MOVEVARS, "Offset x of the camera" );
CVAR_DEFINE_AUTO( vr_offset_y, "0", FCVAR_MOVEVARS, "Offset y of the camera" );
CVAR_DEFINE_AUTO( vr_offset_z, "0", FCVAR_MOVEVARS, "Offset z of the camera" );
CVAR_DEFINE_AUTO( vr_player_dir_x, "0", FCVAR_MOVEVARS, "Direction x of the player" );
CVAR_DEFINE_AUTO( vr_player_dir_y, "0", FCVAR_MOVEVARS, "Direction y of the player" );
CVAR_DEFINE_AUTO( vr_player_dir_z, "0", FCVAR_MOVEVARS, "Direction z of the player" );
CVAR_DEFINE_AUTO( vr_player_pos_x, "0", FCVAR_MOVEVARS, "Position x of the player" );
CVAR_DEFINE_AUTO( vr_player_pos_y, "0", FCVAR_MOVEVARS, "Position y of the player" );
CVAR_DEFINE_AUTO( vr_player_pos_z, "0", FCVAR_MOVEVARS, "Position z of the player" );
CVAR_DEFINE_AUTO( vr_player_pitch, "0", FCVAR_MOVEVARS, "Pinch angle of the player" );
CVAR_DEFINE_AUTO( vr_player_yaw, "0", FCVAR_MOVEVARS, "Yaw angle of the player" );
CVAR_DEFINE_AUTO( vr_shielded, "0", FCVAR_MOVEVARS, "Player is covered by shield" );
CVAR_DEFINE_AUTO( vr_spectator, "0", FCVAR_MOVEVARS, "Is the game in spectator mode?" );
CVAR_DEFINE_AUTO( vr_stereo_side, "0", FCVAR_MOVEVARS, "Eye being drawn" );
CVAR_DEFINE_AUTO( vr_vignette_amount, "0", FCVAR_MOVEVARS, "Amount of rendered vignette" );
CVAR_DEFINE_AUTO( vr_weapon_anim, "1", FCVAR_MOVEVARS, "Disabling animations for motion controls" );
CVAR_DEFINE_AUTO( vr_weapon_calibration_on, "0", FCVAR_MOVEVARS, "Tool to calibrate weapons" );
CVAR_DEFINE_AUTO( vr_weapon_calibration_update, "0", FCVAR_MOVEVARS, "Information to update calibration" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_name, "", FCVAR_MOVEVARS, "Current weapon name" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_pitch, "0", FCVAR_MOVEVARS, "Weapon pivot pitch" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_scale, "0", FCVAR_MOVEVARS, "Weapon pivot scale" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_yaw, "0", FCVAR_MOVEVARS, "Weapon pivot yaw" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_x, "0", FCVAR_MOVEVARS, "Weapon pivot position x" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_y, "0", FCVAR_MOVEVARS, "Weapon pivot position y" );
CVAR_DEFINE_AUTO( vr_weapon_pivot_z, "0", FCVAR_MOVEVARS, "Weapon pivot position z" );
CVAR_DEFINE_AUTO( vr_weapon_roll, "0", FCVAR_MOVEVARS, "Weapon roll angle" );
CVAR_DEFINE_AUTO( vr_weapon_throw_active, "0", FCVAR_MOVEVARS, "Throwing grenade active" );
CVAR_DEFINE_AUTO( vr_weapon_throw_pitch, "0", FCVAR_MOVEVARS, "Throwing grenade pitch angle" );
CVAR_DEFINE_AUTO( vr_weapon_throw_yaw, "0", FCVAR_MOVEVARS, "Throwing grenade yaw angle" );
CVAR_DEFINE_AUTO( vr_weapon_x, "0", FCVAR_MOVEVARS, "Weapon position x" );
CVAR_DEFINE_AUTO( vr_weapon_y, "0", FCVAR_MOVEVARS, "Weapon position y" );
CVAR_DEFINE_AUTO( vr_weapon_z, "0", FCVAR_MOVEVARS, "Weapon position z" );
CVAR_DEFINE_AUTO( vr_xhair_x, "0", FCVAR_MOVEVARS, "Cross-hair 2d position x" );
CVAR_DEFINE_AUTO( vr_xhair_y, "0", FCVAR_MOVEVARS, "Cross-hair 2d position y" );
CVAR_DEFINE_AUTO( vr_superzoomed, "0", FCVAR_MOVEVARS, "Flag if the scene super zoomed" );
CVAR_DEFINE_AUTO( vr_zoomed, "0", FCVAR_MOVEVARS, "Flag if the scene zoomed" );
CVAR_DEFINE_AUTO( vr_zoomfix, "0", FCVAR_MOVEVARS, "Flag that zoom has to be adjusted" );
CVAR_DEFINE_AUTO( vr_zoom_by_motion, "0", FCVAR_MOVEVARS, "Flag if the scene zoomed by motion" );


CVAR_DEFINE_AUTO( vr_6dof, "1", FCVAR_ARCHIVE, "Use 6DoF world tracking" );
CVAR_DEFINE_AUTO( vr_arm_length, "3", FCVAR_ARCHIVE, "Arm length constant" );
CVAR_DEFINE_AUTO( vr_button_alt, "vr_button_x", FCVAR_ARCHIVE, "Button to active alternative mapping" );
CVAR_DEFINE_AUTO( vr_button_a, "+duck", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_b, "+jump", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_x, "drop", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_y, "+vr_scoreboard", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_grip_left, "+voicerecord", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_press_left, "exec touch/cmd/cmd", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_trigger_left, "+use;buy", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_grip_right, "+reload", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dup_right, "+vr_weapon_prev", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_ddown_right, "+vr_weapon_next", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dleft_right, "+vr_left", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dright_right, "+vr_right", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_press_right, "+attack2", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_trigger_right, "+attack", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_a_alt, "", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_b_alt, "", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_x_alt, "", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_y_alt, "", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_grip_left_alt, "+voicerecord", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_press_left_alt, "", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_trigger_left_alt, "impulse 201", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_grip_right_alt, "nightvision;toggle_light", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dup_right_alt, "+vr_weapon_prev", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_ddown_right_alt, "+vr_weapon_next", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dleft_right_alt, "+vr_left", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_dright_right_alt, "+vr_right", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_thumbstick_press_right_alt, "touch_hide say;touch_hide say2;messagemode", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_button_trigger_right_alt, "drop", FCVAR_ARCHIVE, "Controller mapping" );
CVAR_DEFINE_AUTO( vr_haptics_enable, "1", FCVAR_ARCHIVE, "Flag if haptics are enabled" );
CVAR_DEFINE_AUTO( vr_motion_activation, "3", FCVAR_ARCHIVE, "0=disable, 1=using use button, 2=stretch arm, 3=both" );
CVAR_DEFINE_AUTO( vr_thumbstick_deadzone_left, "0.15", FCVAR_ARCHIVE, "Deadzone of thumbstick to filter drift" );
CVAR_DEFINE_AUTO( vr_thumbstick_deadzone_right, "0.8", FCVAR_ARCHIVE, "Deadzone of thumbstick to filter drift" );
CVAR_DEFINE_AUTO( vr_thumbstick_swap, "0", FCVAR_ARCHIVE, "1=swap thumbsticks (left/right)" );
CVAR_DEFINE_AUTO( vr_turn_angle, "45", FCVAR_ARCHIVE, "Angle to rotate by a thumbstick" );
CVAR_DEFINE_AUTO( vr_turn_type, "0", FCVAR_ARCHIVE, "0 = snap turn, 1 = smooth turn" );
CVAR_DEFINE_AUTO( vr_msaa, "0", FCVAR_ARCHIVE, "Game rendering subpixel rendering" );
CVAR_DEFINE_AUTO( vr_refreshrate, "0", FCVAR_ARCHIVE, "1=force 90hz refresh rate" );
CVAR_DEFINE_AUTO( vr_righthand, "1", FCVAR_ARCHIVE, "Use right hand mapping" );
CVAR_DEFINE_AUTO( vr_supersampling, "1.1", FCVAR_ARCHIVE, "Game rendering resolution" );
CVAR_DEFINE_AUTO( vr_vignette, "0", FCVAR_ARCHIVE, "1=enable comfort vignette" );
CVAR_DEFINE_AUTO( vr_walkdirection, "1", FCVAR_ARCHIVE, "0=direction from controller, 1=direction from HMD" );
CVAR_DEFINE_AUTO( vr_weapon_restore, "1", FCVAR_ARCHIVE, "0=keep models/alignment, 1=restore models/alignment" );
CVAR_DEFINE_AUTO( vr_worldscale, "30", FCVAR_ARCHIVE, "Sets the world scale for stereo separation" );
CVAR_DEFINE_AUTO( vr_xhair, "1", FCVAR_ARCHIVE, "Cross-hair rendering" );
CVAR_DEFINE_AUTO( vr_hud_distance, "2", FCVAR_ARCHIVE, "Desktop HUD plane distance in metres" );
CVAR_DEFINE_AUTO( vr_hud_width, "1.6", FCVAR_ARCHIVE, "Desktop HUD plane width in metres" );

vec3_t vr_hmd_offset = {};
vec2_t vr_input = {};
bool vr_ducked = false;
#ifndef ANDROID
static float vr_tracking_yaw;
static bool vr_tracking_anchored;
#endif
extern bool sdl_keyboard_requested;

void Host_VRInit( void )
{
	const char* eyeVariables[] = {"vr_eye_x", "vr_eye_y", "vr_eye_z", "vr_eye_pitch", "vr_eye_yaw", "vr_eye_roll",
		"vr_eye_left", "vr_eye_right", "vr_eye_down", "vr_eye_up", "vr_eye_qx", "vr_eye_qy", "vr_eye_qz"};
	Cvar_RegisterVariable(&vr_enabled);
	Cvar_RegisterVariable(&vr_hud_distance);
	Cvar_RegisterVariable(&vr_hud_width);
	Cvar_Get("vr_eye_qw", "1", FCVAR_READ_ONLY, "Eye orientation relative to head");
	Cvar_Get("vr_menu_slots", "0", 0, "Valid slots in the client text menu");
	Cvar_Get("vr_menu_selection", "0", 0, "Selected client menu slot");
	for (unsigned int i = 0; i < sizeof(eyeVariables)/sizeof(eyeVariables[0]); ++i)
		Cvar_Get(eyeVariables[i], "0", FCVAR_READ_ONLY, "Current OpenXR eye geometry");
	Cvar_RegisterVariable( &vr_camera_x );
	Cvar_RegisterVariable( &vr_camera_y );
	Cvar_RegisterVariable( &vr_camera_z );
	Cvar_RegisterVariable( &vr_force2d );
	Cvar_RegisterVariable( &vr_gamemode );
	Cvar_RegisterVariable( &vr_hand_active );
	Cvar_RegisterVariable( &vr_hand_click );
	Cvar_RegisterVariable( &vr_hand_stretch );
	Cvar_RegisterVariable( &vr_hand_x );
	Cvar_RegisterVariable( &vr_hand_y );
	Cvar_RegisterVariable( &vr_hand_z );
	Cvar_RegisterVariable( &vr_hand_pitch );
	Cvar_RegisterVariable( &vr_hand_yaw );
	Cvar_RegisterVariable( &vr_hand_roll );
	Cvar_RegisterVariable( &vr_hand_swap );
	Cvar_RegisterVariable( &vr_haptics_enable );
	Cvar_RegisterVariable( &vr_haptics_weapon );
	Cvar_RegisterVariable( &vr_haptics_yaw );
	Cvar_RegisterVariable( &vr_hmd_pitch );
	Cvar_RegisterVariable( &vr_hmd_yaw );
	Cvar_RegisterVariable( &vr_hmd_roll );
	Cvar_RegisterVariable( &vr_ladder );
	Cvar_RegisterVariable( &vr_msaa );
	Cvar_RegisterVariable( &vr_motion_activation );
	Cvar_RegisterVariable( &vr_offset_x );
	Cvar_RegisterVariable( &vr_offset_y );
	Cvar_RegisterVariable( &vr_offset_z );
	Cvar_RegisterVariable( &vr_player_dir_x );
	Cvar_RegisterVariable( &vr_player_dir_y );
	Cvar_RegisterVariable( &vr_player_dir_z );
	Cvar_RegisterVariable( &vr_player_pos_x );
	Cvar_RegisterVariable( &vr_player_pos_y );
	Cvar_RegisterVariable( &vr_player_pos_z );
	Cvar_RegisterVariable( &vr_player_pitch );
	Cvar_RegisterVariable( &vr_player_yaw );
	Cvar_RegisterVariable( &vr_refreshrate );
	Cvar_RegisterVariable( &vr_righthand );
	Cvar_RegisterVariable( &vr_shielded );
	Cvar_RegisterVariable( &vr_spectator );
	Cvar_RegisterVariable( &vr_stereo_side );
	Cvar_RegisterVariable( &vr_thumbstick_deadzone_left );
	Cvar_RegisterVariable( &vr_thumbstick_deadzone_right );
	Cvar_RegisterVariable( &vr_thumbstick_swap );
	Cvar_RegisterVariable( &vr_turn_angle );
	Cvar_RegisterVariable( &vr_turn_type );
	Cvar_RegisterVariable( &vr_weapon_anim );
	Cvar_RegisterVariable( &vr_weapon_calibration_on );
	Cvar_RegisterVariable( &vr_weapon_calibration_update );
	Cvar_RegisterVariable( &vr_weapon_pivot_name );
	Cvar_RegisterVariable( &vr_weapon_pivot_pitch );
	Cvar_RegisterVariable( &vr_weapon_pivot_scale );
	Cvar_RegisterVariable( &vr_weapon_pivot_yaw );
	Cvar_RegisterVariable( &vr_weapon_pivot_x );
	Cvar_RegisterVariable( &vr_weapon_pivot_y );
	Cvar_RegisterVariable( &vr_weapon_pivot_z );
	Cvar_RegisterVariable( &vr_weapon_roll );
	Cvar_RegisterVariable( &vr_weapon_throw_active );
	Cvar_RegisterVariable( &vr_weapon_throw_pitch );
	Cvar_RegisterVariable( &vr_weapon_throw_yaw );
	Cvar_RegisterVariable( &vr_weapon_x );
	Cvar_RegisterVariable( &vr_weapon_y );
	Cvar_RegisterVariable( &vr_weapon_z );
	Cvar_RegisterVariable( &vr_supersampling );
	Cvar_RegisterVariable( &vr_vignette );
	Cvar_RegisterVariable( &vr_vignette_amount );
	Cvar_RegisterVariable( &vr_walkdirection );
	Cvar_RegisterVariable( &vr_weapon_restore );
	Cvar_RegisterVariable( &vr_worldscale );
	Cvar_RegisterVariable( &vr_xhair );
	Cvar_RegisterVariable( &vr_xhair_x );
	Cvar_RegisterVariable( &vr_xhair_y );

	Cvar_RegisterVariable( &vr_6dof );
	Cvar_RegisterVariable( &vr_arm_length );
	Cvar_RegisterVariable( &vr_button_alt );
	Cvar_RegisterVariable( &vr_button_a );
	Cvar_RegisterVariable( &vr_button_b );
	Cvar_RegisterVariable( &vr_button_x );
	Cvar_RegisterVariable( &vr_button_y );
	Cvar_RegisterVariable( &vr_button_grip_left );
	Cvar_RegisterVariable( &vr_button_thumbstick_press_left );
	Cvar_RegisterVariable( &vr_button_trigger_left );
	Cvar_RegisterVariable( &vr_button_grip_right );
	Cvar_RegisterVariable( &vr_button_thumbstick_dup_right );
	Cvar_RegisterVariable( &vr_button_thumbstick_ddown_right );
	Cvar_RegisterVariable( &vr_button_thumbstick_dleft_right );
	Cvar_RegisterVariable( &vr_button_thumbstick_dright_right );
	Cvar_RegisterVariable( &vr_button_thumbstick_press_right );
	Cvar_RegisterVariable( &vr_button_trigger_right );
	Cvar_RegisterVariable( &vr_button_a_alt );
	Cvar_RegisterVariable( &vr_button_b_alt );
	Cvar_RegisterVariable( &vr_button_x_alt );
	Cvar_RegisterVariable( &vr_button_y_alt );
	Cvar_RegisterVariable( &vr_button_grip_left_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_press_left_alt );
	Cvar_RegisterVariable( &vr_button_trigger_left_alt );
	Cvar_RegisterVariable( &vr_button_grip_right_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_dup_right_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_ddown_right_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_dleft_right_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_dright_right_alt );
	Cvar_RegisterVariable( &vr_button_thumbstick_press_right_alt );
	Cvar_RegisterVariable( &vr_button_trigger_right_alt );
	Cvar_RegisterVariable( &vr_superzoomed );
	Cvar_RegisterVariable( &vr_zoomed );
	Cvar_RegisterVariable( &vr_zoomfix );
	Cvar_RegisterVariable( &vr_zoom_by_motion );
}

bool Host_VRInitFrame( void )
{
	static bool firstFrame = true;
	engine_t* engine = VR_GetEngine();
	if (firstFrame || !engine->appState.Session) {
		VR_EnterVR(engine);
		IN_VRInit(engine);
		VR_Haptics_Enable();
		firstFrame = false;
	}
	if (!VR_GetConfig(VR_CONFIG_VIEWPORT_VALID)) {
		VR_InitRenderer(engine, false);
		VR_SetConfigFloat(VR_CONFIG_CANVAS_DISTANCE, 5);
		VR_SetConfig(VR_CONFIG_VIEWPORT_VALID, true);
	}
	bool force2d = Cvar_VariableValue("vr_force2d") > 0.5f;
	bool gameMode = !force2d && (!host.mouse_visible || Touch_MenuActive()) && cls.state == ca_active && cls.key_dest == key_game;
	bool mapOverview = Cvar_VariableValue("vr_spectator") > 4.5f;
	int vrMode = mapOverview ? VR_MODE_MONO_6DOF : VR_MODE_STEREO_6DOF;
	int mode = gameMode ? vrMode : VR_MODE_MONO_SCREEN;
	VR_SetConfig(VR_CONFIG_MODE, mode);
	Cvar_LazySet("vr_gamemode", gameMode ? 1 : 0);

	/* Resize/reconfigure before locating this frame's immutable eye poses. */
	Host_VRConfig();
	bool ready = VR_InitFrame(engine);
	if (engine->appState.ExitRequested) Sys_Quit("OpenXR session exited");
	if (!ready) Host_VRButtonMapping(Cvar_VariableValue("cl_righthand") < 0.5f, 0, 0);
	if (!ready && !engine->appState.SessionActive) SDL_Delay(10);
	return ready;
}

static void Host_VRSetEyeValue(const char* name, float value)
{
	char text[32];
	/* Q_atof does not parse scientific notation. Near-zero tracking noise
	 * formatted with %g becomes metres of eye displacement when read back. */
	VR_FormatPoseValue(text, value);
	Cvar_FullSet(name, text, FCVAR_READ_ONLY);
}

void Host_VRClientFrame( void )
{
	engine_t* engine = VR_GetEngine();
	for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
		XrPosef head = VR_GetHead();
		XrPosef view = VR_GetView(eye);
		XrVector3f delta = {view.position.x - head.position.x, view.position.y - head.position.y, view.position.z - head.position.z};
		XrQuaternionf inverseHead = XrQuaternionf_Inverse(head.orientation);
		XrVector3f offset = XrQuaternionf_Rotate(inverseHead, delta);
		XrQuaternionf relativeEye = XrQuaternionf_Multiply(inverseHead, view.orientation);
		Host_VRSetEyeValue("vr_eye_qx", relativeEye.x);
		Host_VRSetEyeValue("vr_eye_qy", relativeEye.y);
		Host_VRSetEyeValue("vr_eye_qz", relativeEye.z);
		Host_VRSetEyeValue("vr_eye_qw", relativeEye.w);
		XrVector3f angles = XrQuaternionf_ToEulerAngles(XrQuaternionf_Multiply(inverseHead, view.orientation));
#ifndef ANDROID
		XrQuaternionf worldYaw = XrQuaternionf_CreateFromVectorAngle((XrVector3f){0, 1, 0}, ToRadians(vr_tracking_yaw));
		angles = XrQuaternionf_ToEulerAngles(XrQuaternionf_Multiply(worldYaw, view.orientation));
#endif
		XrFovf fov = VR_GetEyeFov(eye);
		Host_VRSetEyeValue("vr_eye_x", offset.x);
		Host_VRSetEyeValue("vr_eye_y", offset.y);
		Host_VRSetEyeValue("vr_eye_z", offset.z);
		Host_VRSetEyeValue("vr_eye_pitch", angles.x);
		Host_VRSetEyeValue("vr_eye_yaw", angles.y);
		Host_VRSetEyeValue("vr_eye_roll", angles.z);
		Host_VRSetEyeValue("vr_eye_left", tanf(fov.angleLeft));
		Host_VRSetEyeValue("vr_eye_right", tanf(fov.angleRight));
		Host_VRSetEyeValue("vr_eye_down", tanf(fov.angleDown));
		Host_VRSetEyeValue("vr_eye_up", tanf(fov.angleUp));
		Cvar_SetValue("vr_stereo_side", eye);
		if (getenv("CSVR_XR_TRACE") && engine->frameIndex >= 649 && engine->frameIndex <= 651)
			printf("XR geometry frame=%llu eye=%d pose=(%.4f %.4f %.4f) q=(%.4f %.4f %.4f %.4f) head=(%.4f %.4f %.4f) worldYaw=%.3f angles=(%.3f %.3f %.3f) offset=(%.4f %.4f %.4f) fov=(%.4f %.4f %.4f %.4f)\n", (unsigned long long)engine->frameIndex, eye, view.position.x, view.position.y, view.position.z, view.orientation.x, view.orientation.y, view.orientation.z, view.orientation.w, head.position.x, head.position.y, head.position.z, Cvar_VariableValue("vr_hmd_yaw") - XrQuaternionf_ToEulerAngles(head.orientation).y, angles.x, angles.y, angles.z, offset.x, offset.y, offset.z, fov.angleLeft, fov.angleRight, fov.angleDown, fov.angleUp);
		VR_BeginFrame(engine, eye);
		Host_ClientFrame();
		VR_EndFrame(engine, eye);
	}
	VR_FinishFrame(engine);
	VR_Haptics_EndFrame();
}

void Host_VRInputFrame( void )
{
	// Get VR input
	bool rightHanded = Cvar_VariableValue("cl_righthand") > 0;
	int primaryController = rightHanded ? 1 : 0;
	int secondaryController = rightHanded ? 0 : 1;
	XrPosef hmd = VR_GetHead();
	XrPosef hand = IN_VRGetPose(secondaryController);
	XrPosef weapon = IN_VRGetPose(primaryController);
#ifndef ANDROID
	/* An untracked controller is not an identity-pose aiming device. */
	if (!IN_VRIsActive(primaryController)) weapon = hmd;
	if (!IN_VRIsActive(secondaryController)) hand = hmd;
#endif
	XrVector3f angles = XrQuaternionf_ToEulerAngles(weapon.orientation);
	bool cursorActive = IN_VRIsActive(primaryController);
	unsigned int lbuttons = IN_VRGetButtonState(secondaryController);
	unsigned int rbuttons = IN_VRGetButtonState(primaryController);
	XrVector2f left = IN_VRGetJoystickState(secondaryController);
	XrVector2f right = IN_VRGetJoystickState(primaryController);
#ifndef ANDROID
	float turnDeadzone = Cvar_VariableValue("vr_thumbstick_deadzone_right");
	if (fabsf(right.x) < turnDeadzone) right.x = 0;
	if (fabsf(right.y) < turnDeadzone) right.y = 0;
#endif

	// Convert input data
	bool zoomed = Cvar_VariableValue("vr_zoomed") > 0;
	bool zoomfix = Cvar_VariableValue("vr_zoomfix") > 0;
	bool superzoomed = Cvar_VariableValue("vr_superzoomed") > 0;
	XrVector3f weaponEuler = XrQuaternionf_ToEulerAngles(zoomed && !zoomfix ? hmd.orientation : weapon.orientation);
	XrVector3f handEuler = XrQuaternionf_ToEulerAngles(hand.orientation);
	XrVector3f hmdEuler = XrQuaternionf_ToEulerAngles(hmd.orientation);
	vec3_t hmdAngles = {hmdEuler.x, hmdEuler.y, hmdEuler.z};
	vec3_t handAngles = {handEuler.x, handEuler.y, handEuler.z};
	vec3_t weaponAngles = {weaponEuler.x, weaponEuler.y, weaponEuler.z};
	vec3_t weaponPosition = {weapon.position.x, weapon.position.y, weapon.position.z};
	vec3_t handPosition = {hand.position.x, hand.position.y, hand.position.z};
	vec3_t hmdPosition = {hmd.position.x, hmd.position.y, hmd.position.z};
	bool handActive = Host_VRAdjustInput(handAngles, handPosition, hmdAngles, hmdPosition, weaponAngles, weaponPosition);

	// Menu control
	vec2_t cursor = {};
	bool gameMode = Cvar_VariableValue("vr_gamemode") > 0.5f;
	Host_VRCursor(cursorActive && (cls.key_dest != key_console), angles.x, angles.y, cursor);
	bool pressedInUI = Host_VRMenuInput(cursorActive, gameMode, !rightHanded, lbuttons, rbuttons, cursor);
#ifndef ANDROID
	if (gameMode && cl.paused) {
		left = right = (XrVector2f){0, 0};
		lbuttons = rbuttons = 0;
	}
	if (gameMode && pressedInUI) {
		Host_VRButtonMapping(!rightHanded, 0, 0);
		clgame.dllFuncs.pfnMoveEvent(0, 0);
		Host_VRRotations(zoomed && !zoomfix, handAngles, hmdAngles, hmdPosition, weaponAngles, 0, 0);
		return;
	}
#endif

	// Do not pass button actions which started in UI
	if (gameMode && pressedInUI) {
		lbuttons = 0;
		rbuttons = 0;
	}

	// In-game input
	if (gameMode) {
		Host_VRBUttonMaskThumbstick(left.x, left.y, &lbuttons);
		Host_VRButtonMapping(!rightHanded, lbuttons, rbuttons);
		if (Host_VRWeaponCalibration(right.x, right.y)) {
			right.x = 0;
			right.y = 0;
#ifdef ANDROID
		} else {
			right.x = vr_input[0];
			right.y = vr_input[1];
#endif
		}
		if (!cl.paused) {
		Host_VRMotionControls(zoomed, superzoomed, handActive, hmdAngles, handPosition, hmdPosition, weaponPosition);
		Host_VRMovementPlayer(hmdAngles, hmdPosition, weaponAngles, left.x, left.y);
		Host_VRMovementEntity(zoomed && !zoomfix, handPosition, hmdAngles, hmdPosition, weaponPosition);
		}
		Host_VRRotations(zoomed && !zoomfix, handAngles, hmdAngles, hmdPosition, weaponAngles, right.x, right.y);
	} else {
		// Measure player when not in game mode
#ifndef ANDROID
		vr_tracking_anchored = false;
#endif
		vr_hmd_offset[2] = hmd.position.y;

		// No game actions when UI is shown
		Host_VRButtonMapping(!rightHanded, 0, 0);
		clgame.dllFuncs.pfnMoveEvent( 0, 0 );
		Cvar_SetValue("vr_vignette_amount", 0);
	}
	Host_VRHaptics( rightHanded, weaponAngles );
}

bool Host_VRAdjustInput( vec3_t handAngles, vec3_t handPosition, vec3_t hmdAngles, const vec3_t hmdPosition, vec3_t weaponAngles, vec3_t weaponPosition )
{
	// Single hand mapping when shield used
	if (Cvar_VariableValue("vr_shielded") > 0) {
		VectorCopy(handAngles, weaponAngles);
		VectorCopy(handPosition, weaponPosition);
	}

	// Swap left-right hand based on two hand weapon status
	if (strcmp(Cvar_VariableString("vr_weapon_pivot_name"), "models/v_elite.mdl") == 0) {
		if (Cvar_VariableValue("vr_hand_swap") > 0.5f) {
			vec3_t angles, position;
			VectorCopy(handAngles, angles);
			VectorCopy(handPosition, position);
			VectorCopy(weaponAngles, handAngles);
			VectorCopy(weaponPosition, handPosition);
			VectorCopy(angles, weaponAngles);
			VectorCopy(position, weaponPosition);
		}
	}

	// Two hand weapons and hand pointer
	bool armStretching = Cvar_VariableValue("vr_motion_activation") > 1.5f;
	bool buttonActivator1 = fabs(Cvar_VariableValue("vr_motion_activation") - 1) < 0.1f;
	bool buttonActivator2 = fabs(Cvar_VariableValue("vr_motion_activation") - 3) < 0.1f;
	bool handClick = Cvar_VariableValue("vr_hand_click") > 0.5f;
	bool handStretch = Cvar_VariableValue("vr_hand_stretch") > 0.5f;
	bool handActive = (((buttonActivator1 || buttonActivator2) && handClick) || (armStretching && handStretch));
	if (handActive) {
		float dirX = hmdPosition[0] - handPosition[0];
		float dirY = hmdPosition[2] - handPosition[2];
		float dirZ = hmdPosition[1] - handPosition[1] - 0.15f;
		float dir = sqrt(dirX * dirX + dirY * dirY);
		weaponAngles[PITCH] = RAD2DEG(sin(dirZ / dir));
		weaponAngles[YAW] = RAD2DEG(atan2(dirX, dirY));
	}
	Cvar_SetValue("vr_hand_active", handActive ? 1 : 0);

	// Change weapon angles if throwing a grenade
	if (Cvar_VariableValue("vr_weapon_throw_active") > 0.5f) {
		weaponAngles[PITCH] = Cvar_VariableValue("vr_weapon_throw_pitch");
		weaponAngles[YAW] = Cvar_VariableValue("vr_weapon_throw_yaw");
		weaponAngles[ROLL] = 0;
	}

	// Spectator behavior
	if (Cvar_VariableValue("vr_spectator") > 0.5f) {
		bool freecam = fabs(Cvar_VariableValue("vr_spectator") - 3) < 0.1f;
		weaponAngles[PITCH] = freecam ? hmdAngles[PITCH] : 0;
		weaponAngles[YAW] = freecam ? hmdAngles[YAW] : 0;
		weaponAngles[ROLL] = freecam ? hmdAngles[ROLL] : 0;
	}

	return handActive;
}

void Host_VRButtonMap( unsigned int button, int currentButtons, int lastButtons, const char* name, bool alt )
{
	// Detect if action should be called
	bool down = currentButtons & button;
	bool wasDown = lastButtons & button;
	bool process = false;
	bool invert = false;
	if (down && !wasDown) {
		process = true;
	} else if (!down && wasDown) {
		process = true;
		invert = true;
	}

	// Process commands separated by a semicolon
	if (process) {
		int index = 0;
		static char command[256];
		static char fullname[64];
		sprintf(fullname, "%s%s", name, alt ? "_alt" : "");
		const char* action = Cvar_VariableString(fullname);
		for (int i = 0; i <= strlen(action); i++) {
			if ((action[i] == ';') || (action[i] == '\000')) {
				command[index++] = '\n';
				command[index] = '\000';
				if (invert) {
					if (command[0] == '+') {
						command[0] = '-';
						Host_VRCustomCommand( command );
					}
				} else {
					Host_VRCustomCommand( command );
				}
				index = 0;
			} else {
				command[index++] = action[i];
			}
		}
	}
}

void Host_VRButtonMapping( bool swapped, unsigned int lbuttons, unsigned int rbuttons )
{
	int leftPrimaryButton = swapped ? ovrButton_A : ovrButton_X;
	int leftSecondaryButton = swapped ? ovrButton_B : ovrButton_Y;
	int rightPrimaryButton = !swapped ? ovrButton_A : ovrButton_X;
	int rightSecondaryButton = !swapped ? ovrButton_B : ovrButton_Y;

	// Detect if alt mapping should be used
	bool alt = false;
	const char* altButton = Cvar_VariableString("vr_button_alt");
	if ((strcmp(altButton, "vr_button_x") == 0) && (lbuttons & leftPrimaryButton)) alt = true;
	else if ((strcmp(altButton, "vr_button_y") == 0) && (lbuttons & leftSecondaryButton)) alt = true;
	else if ((strcmp(altButton, "vr_button_trigger_left") == 0) && (lbuttons & ovrButton_Trigger)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_press_left") == 0) && (lbuttons & ovrButton_Joystick)) alt = true;
	else if ((strcmp(altButton, "vr_button_grip_left") == 0) && (lbuttons & ovrButton_GripTrigger)) alt = true;
	else if ((strcmp(altButton, "vr_button_a") == 0) && (rbuttons & rightPrimaryButton)) alt = true;
	else if ((strcmp(altButton, "vr_button_b") == 0) && (rbuttons & rightSecondaryButton)) alt = true;
	else if ((strcmp(altButton, "vr_button_trigger_right") == 0) && (rbuttons & ovrButton_Trigger)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_dup_right") == 0) && (rbuttons & ovrButton_Up)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_ddown_right") == 0) && (rbuttons & ovrButton_Down)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_dleft_right") == 0) && (rbuttons & ovrButton_Left)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_dright_right") == 0) && (rbuttons & ovrButton_Right)) alt = true;
	else if ((strcmp(altButton, "vr_button_thumbstick_press_right") == 0) && (rbuttons & ovrButton_Joystick)) alt = true;
	else if ((strcmp(altButton, "vr_button_grip_right") == 0) && (rbuttons & ovrButton_GripTrigger)) alt = true;

	// Apply button actions
	static int lastlbuttons = 0;
	Host_VRButtonMap(leftPrimaryButton, lbuttons, lastlbuttons, "vr_button_x", alt);
	Host_VRButtonMap(leftSecondaryButton, lbuttons, lastlbuttons, "vr_button_y", alt);
	Host_VRButtonMap(ovrButton_Trigger, lbuttons, lastlbuttons, "vr_button_trigger_left", alt);
	Host_VRButtonMap(ovrButton_Joystick, lbuttons, lastlbuttons, "vr_button_thumbstick_press_left", alt);
	Host_VRButtonMap(ovrButton_GripTrigger, lbuttons, lastlbuttons, "vr_button_grip_left", alt);
	lastlbuttons = lbuttons;
	static int lastrbuttons = 0;
	Host_VRButtonMap(rightPrimaryButton, rbuttons, lastrbuttons, "vr_button_a", alt);
	Host_VRButtonMap(rightSecondaryButton, rbuttons, lastrbuttons, "vr_button_b", alt);
	Host_VRButtonMap(ovrButton_Trigger, rbuttons, lastrbuttons, "vr_button_trigger_right", alt);
	Host_VRButtonMap(ovrButton_Up, rbuttons, lastrbuttons, "vr_button_thumbstick_dup_right", alt);
	Host_VRButtonMap(ovrButton_Down, rbuttons, lastrbuttons, "vr_button_thumbstick_ddown_right", alt);
	Host_VRButtonMap(ovrButton_Left, rbuttons, lastrbuttons, "vr_button_thumbstick_dleft_right", alt);
	Host_VRButtonMap(ovrButton_Right, rbuttons, lastrbuttons, "vr_button_thumbstick_dright_right", alt);
	Host_VRButtonMap(ovrButton_Joystick, rbuttons, lastrbuttons, "vr_button_thumbstick_press_right", alt);
	Host_VRButtonMap(ovrButton_GripTrigger, rbuttons, lastrbuttons, "vr_button_grip_right", alt);
	lastrbuttons = rbuttons;
}

void Host_VRBUttonMaskThumbstick( float x, float y, unsigned int* buttons )
{
	float deadzone = Cvar_VariableValue("vr_thumbstick_deadzone_right");

	static Uint64 lastActive;
	Uint64 now = SDL_GetTicks64();
	if ((fabs(x) > deadzone) || (fabs(y) > deadzone)) lastActive = now;
	if (now - lastActive < 1000) *buttons &= ~ovrButton_Joystick;
}

bool Host_VRConfig(void)
{
	// Update refresh rate if needed
	static int lastRefreshRate = 72;
	int currentRefreshRate = Cvar_VariableValue("vr_refreshrate") > 0.5f ? 90 : 72;
	if (lastRefreshRate != currentRefreshRate) {
		VR_SetRefreshRate(currentRefreshRate);
		lastRefreshRate = currentRefreshRate;
	}

	// Update viewport if needed
	static float lastMSAA = 1;
	static float lastSupersampling = 1;
	bool gameMode = Cvar_VariableValue("vr_gamemode") > 0.5f;
	float currentMSAA = Cvar_VariableValue("vr_msaa") + 1.0f;
	float currentSupersampling = Cvar_VariableValue("vr_supersampling");
	if (gameMode && ((fabs(lastMSAA - currentMSAA) > 0.01f) || (fabs(lastSupersampling - currentSupersampling) > 0.01f))) {
		int width, height;
		VR_SetConfig(VR_CONFIG_VIEWPORT_MSAA, currentMSAA);
		VR_SetConfigFloat(VR_CONFIG_VIEWPORT_SUPERSAMPLING, currentSupersampling);
		VR_InitRenderer(VR_GetEngine(), false);
		VR_GetResolution(VR_GetEngine(), &width, &height);
		VID_SaveWindowSize( width, height, true );
		lastSupersampling = currentSupersampling;
		lastMSAA = currentMSAA;
	}
	static bool lastGameMode = true;
	if (lastGameMode != gameMode) {
		int width, height;
		VR_SetConfigFloat(VR_CONFIG_CANVAS_ASPECT, gameMode ? 1.0f : 4.0f / 3.0f);
		VR_GetResolution(VR_GetEngine(), &width, &height);
		VID_SaveWindowSize( width, height, true );
		lastGameMode = gameMode;
	}

	// Update thumbstick deadzone a swapping
	IN_VRSetDeadzone(Cvar_VariableValue("vr_thumbstick_deadzone_right"));
	IN_VRSetSwapThumbsticks(Cvar_VariableValue("vr_thumbstick_swap") > 0.5);

	// Use separate right hand CVAR to not let servers overwrite it
	Cvar_LazySet("cl_righthand", Cvar_VariableValue("vr_righthand"));

	// Ensure VR compatible layout is used
	Cvar_LazySet("con_fontscale", gameMode ? 1.5f : 1.0f);
	Cvar_LazySet("hud_scale", 2);
	Cvar_LazySet("touch_enable", 0);
	Cvar_LazySet("xhair_enable", 1);
#ifndef ANDROID
	/* The mobile VGUI replacement consists of touch-only scripts. */
	Cvar_LazySet("_vgui_menus", 0);
	Cvar_LazySet("_extended_menus", 0);
#endif

	// Ensure voice input is enabled
	Cvar_LazySet("sv_voicequality", 5);
	Cvar_LazySet("voice_inputfromfile", 1);
	Cvar_LazySet("voice_scale", 5);

	return gameMode;
}

void Host_VRCursor( bool cursorActive, float x, float y, vec2_t cursor )
{
	// Calculate cursor position
	float width = (float)VR_GetConfig(VR_CONFIG_VIEWPORT_WIDTH);
	float height = (float)VR_GetConfig(VR_CONFIG_VIEWPORT_HEIGHT);
	float supersampling = VR_GetConfigFloat(VR_CONFIG_VIEWPORT_SUPERSAMPLING);
	float cx = width / 2;
	float cy = height / 2;
	float speed = (cx + cy) / 2;
	float mx = cx - tan(ToRadians(y - VR_GetConfigFloat(VR_CONFIG_MENU_YAW))) * speed;
	float my = cy + tan(ToRadians(x)) * speed * VR_GetConfigFloat(VR_CONFIG_CANVAS_ASPECT);
	cursor[0] = supersampling > 0.1f ? mx * supersampling : mx;
	cursor[1] = supersampling > 0.1f ? my * supersampling : my;

	// Show cursor
	VR_SetConfig(VR_CONFIG_MOUSE_X, cursor[0]);
	VR_SetConfig(VR_CONFIG_MOUSE_Y, height - cursor[1]);
	VR_SetConfig(VR_CONFIG_MOUSE_SIZE, cursorActive ? 8 : 0);
}

void Host_VRCustomCommand( char* action )
{
	static bool attackTriggered = false;
	if (strcmp(action, "+vr_left\n") == 0) vr_input[0] = -1;
	else if (strcmp(action, "-vr_left\n") == 0) vr_input[0] = 0;
	else if (strcmp(action, "+vr_right\n") == 0) vr_input[0] = 1;
	else if (strcmp(action, "-vr_right\n") == 0) vr_input[0] = 0;
	else if (strcmp(action, "+vr_weapon_next\n") == 0) vr_input[1] = -1;
	else if (strcmp(action, "-vr_weapon_next\n") == 0) vr_input[1] = 0;
	else if (strcmp(action, "+vr_weapon_prev\n") == 0) vr_input[1] = 1;
	else if (strcmp(action, "-vr_weapon_prev\n") == 0) vr_input[1] = 0;
	else if (strcmp(action, "+attack2\n") == 0) {
		Cvar_SetValue("vr_zoom_by_motion", 0);
		Cbuf_AddText( action );
	} else if (strcmp(action, "+use\n") == 0) {
		if (strcmp(Cvar_VariableString("vr_weapon_pivot_name"), "models/v_elite.mdl") == 0) {
			Cbuf_AddText("+attack\n");
			attackTriggered = true;
		} else {
			Cvar_SetValue("vr_hand_click", 1);
		}
		Cbuf_AddText( action );
	} else if (strcmp(action, "-use\n") == 0) {
		if (attackTriggered) {
			Cbuf_AddText("-attack\n");
			attackTriggered = false;
		}
		Cvar_SetValue("vr_hand_click", 0);
		Cbuf_AddText( action );
	} else if (strcmp(action, "+duck\n") == 0) {
		Cbuf_AddText( action );
		vr_ducked = true;
	} else if (strcmp(action, "-duck\n") == 0) {
		Cbuf_AddText( action );
		vr_ducked = false;
	} else if (strcmp(action, "+vr_scoreboard\n") == 0) {
		Cbuf_AddText( "showscoreboard2 0.213333 0.835556 0.213333 0.835556 0 0 0 128\n" );
	} else if (strcmp(action, "-vr_scoreboard\n") == 0) {
		Cbuf_AddText( "hidescoreboard2\n" );
	} else {
		Cbuf_AddText( action );
	}
}

void Host_VRHaptics( bool rightHanded, vec3_t weaponAngles )
{
	// Check if haptics are enabled
	if (Cvar_VariableValue("vr_haptics_enable") < 0.5f) {
		return;
	}
	if (Cvar_VariableValue("vr_spectator") > 0.5f) {
		return;
	}

	// Weapon haptics
	bool handSwapped = Cvar_VariableValue("vr_hand_swap") > 0.5f;
	float weaponPower = Cvar_VariableValue("vr_haptics_weapon");
	if (weaponPower > 0) {
		int channel = handSwapped == rightHanded ? 0 : 1;
		IN_VR_Vibrate(weaponPower, channel, 100);
		Cvar_SetValue("vr_haptics_weapon", 0);
		VR_Haptics_Event(weaponPower > 0.2 ? "shotgun_fire" : "pistol_fire", channel + 1, 0, 100, 0, 0);
	}

	//If player position changes then it is like damage
	//In DM if player gets headshot with 100HP damage then it just respawns
	static vec3_t lastPos = {};
	vec3_t currentPos;
	currentPos[0] = Cvar_VariableValue("vr_player_pos_x");
	currentPos[1] = Cvar_VariableValue("vr_player_pos_y");
	currentPos[2] = Cvar_VariableValue("vr_player_pos_z");
	float scale = Cvar_VariableValue("vr_worldscale");

	// Damage haptics
	static int lastHealth = 0;
	if ((lastHealth > cl.local.health) || (VectorDistance(currentPos, lastPos) > scale)) {
		int damage = lastHealth - cl.local.health;
		float duration = (float)(damage) / 50.0f;
		IN_VR_Vibrate(duration, 0, 100);
		IN_VR_Vibrate(duration, 1, 100);
		int flags = damage > 50 ? 4 : 0;
		char* event = damage > 30 ? "shotgun" : "bullet";
		float yaw = RAD2DEG(Cvar_VariableValue("vr_haptics_yaw")) - weaponAngles[YAW];
		VR_Haptics_Event(event, 0, flags, 100, yaw, 0);
	}
	lastHealth = cl.local.health;
	VectorCopy(currentPos, lastPos);
}

bool Host_VRMenuInput( bool cursorActive, bool gameMode, bool swapped, int lbuttons, int rbuttons, vec2_t cursor )
{
#ifndef ANDROID
	static xr_menu_state navigation;
	static bool lastEscape, lastStickClick;
	static unsigned capturedLeft, capturedRight;
	unsigned slots = (unsigned)Cvar_VariableValue("vr_menu_slots") & 0x3ff;
	bool scriptedMenu = Touch_MenuActive();
	bool textMenu = gameMode && !scriptedMenu && slots != 0;
	bool active = !gameMode || textMenu || scriptedMenu;
	if (getenv("CSVR_XR_TRACE")) {
		static int lastContext=-1;
		int context=scriptedMenu ? -4 : textMenu ? (int)slots : active ? -2 : -3;
		if (context!=lastContext) printf("XR menu context=%s validSlots=0x%x paused=%d\n", scriptedMenu ? "scripted" : textMenu ? "numeric" : active ? "desktop" : "game", slots, cl.paused);
		lastContext=context;
	}
	XrVector2f a=IN_VRGetJoystickState(0), b=IN_VRGetJoystickState(1);
	XrVector2f stick = a.x*a.x+a.y*a.y >= b.x*b.x+b.y*b.y ? a : b;
	bool confirm = (rbuttons & ovrButton_Trigger) != 0;
	bool back = ((lbuttons | rbuttons) & (ovrButton_B | ovrButton_Y)) != 0;
	bool escape = ((lbuttons | rbuttons) & ovrButton_Enter) != 0;
	unsigned events = XRMenu_Update(&navigation, active, stick.x, stick.y, confirm, back, Sys_DoubleTime());
	bool stickClick=(lbuttons & ovrButton_Joystick)!=0;
	if (scriptedMenu && ((escape && !lastEscape) || (stickClick && !lastStickClick))) events|=XR_MENU_BACK;
	lastStickClick=stickClick;
	VR_SetConfig(VR_CONFIG_MOUSE_SIZE, 0);
	if (active) { capturedLeft|=lbuttons; capturedRight|=rbuttons; }
	capturedLeft &= lbuttons; capturedRight &= rbuttons;
	if (scriptedMenu) {
		Touch_MenuInput(events);
	} else if (textMenu) {
		int selected=(int)Cvar_VariableValue("vr_menu_selection");
		if (selected<1 || selected>10 || !(slots & (1u<<(selected-1)))) selected=XRMenu_NextSlot(slots, 0, 1);
		if (events & XR_MENU_UP) selected=XRMenu_NextSlot(slots, selected, -1);
		if (events & XR_MENU_DOWN) selected=XRMenu_NextSlot(slots, selected, 1);
		Cvar_SetValue("vr_menu_selection", selected);
		if (events & XR_MENU_CONFIRM) {
			char command[32]; Q_snprintf(command, sizeof(command), "slot%d\n", selected);
			if (getenv("CSVR_XR_TRACE")) printf("XR menu confirm slot=%d\n", selected);
			Cbuf_AddText(command);
		}
		if ((events & XR_MENU_BACK) || (escape && !lastEscape)) Cbuf_AddText("menuselect 10\nclient_buy_close\n");
	} else if (active) {
		const unsigned flags[]={XR_MENU_UP, XR_MENU_DOWN, XR_MENU_LEFT, XR_MENU_RIGHT, XR_MENU_CONFIRM, XR_MENU_BACK};
		const int keys[]={K_UPARROW, K_DOWNARROW, K_LEFTARROW, K_RIGHTARROW, K_ENTER, K_ESCAPE};
		for (int i=0;i<6;++i) if (events & flags[i]) { Key_Event(keys[i], true); Key_Event(keys[i], false); }
	}
	if (!textMenu && !scriptedMenu && escape && !lastEscape) { Key_Event(K_ESCAPE, true); Key_Event(K_ESCAPE, false); }
	lastEscape=escape;
	return active || capturedLeft || capturedRight;
#else
	// Send enter when Keyboard released
	static bool hadFocus = true;
	static bool keyboardShown = false;
	bool hasFocus = host.status != HOST_NOFOCUS_VR;
	if (!hadFocus && hasFocus && keyboardShown) {
		if( cls.key_dest == key_console )
			Key_Console( K_ENTER );
		else if( cls.key_dest != key_menu )
			Key_Message( K_ENTER );
		keyboardShown = false;
		SDL_StopTextInput();
	}
	hadFocus = hasFocus;

	// Get event type
	touchEventType t = event_motion;
	bool down = rbuttons & ovrButton_Trigger;
	static bool pressedInUI = false;
	static bool lastDown = false;
	if (down && !lastDown) {
		t = event_down;
		if (!gameMode) {
			pressedInUI = true;
		}
	} else if (!down && lastDown) {
		t = event_up;
		pressedInUI = false;
	}
	lastDown = down;

	// Send the input event as a touch
	static float initialTouchX = 0;
	static float initialTouchY = 0;
	cursor[0] /= (float)refState.width;
	cursor[1] /= (float)refState.height;
	if (!gameMode && cursorActive) {
		IN_TouchEvent(t, 0, cursor[0], cursor[1], initialTouchX - cursor[0], initialTouchY - cursor[1]);
		if (t == event_up && sdl_keyboard_requested) {
			IN_TouchEvent(event_motion, 0, cursor[0], cursor[1], initialTouchX - cursor[0], initialTouchY - cursor[1]);
			sdl_keyboard_requested = false;
			keyboardShown = true;
			SDL_StartTextInput();
		}
	}
	initialTouchX = cursor[0];
	initialTouchY = cursor[1];

	// Escape key
	int buttons = swapped ? rbuttons : lbuttons;
	bool escape = buttons & ovrButton_Enter;
	static bool lastEscape = false;
	if (escape && !lastEscape) {
		Key_Event(K_ESCAPE, true);
		Key_Event(K_ESCAPE, false);
	}
	lastEscape = escape;

	// Console scrolling
	if (cls.key_dest == key_console) {
		if (rbuttons & ovrButton_Down) {
			Con_PageDown(1);
		} else if (rbuttons & ovrButton_Up) {
			Con_PageUp(1);
		}
	}

	// Thumbstick close key
	bool thumbstick = lbuttons & ovrButton_Joystick;
	static bool lastThumbstick = false;
	if (thumbstick && !lastThumbstick) {
		Cbuf_AddText( "touch_setclientonly 0\n" );
		if (!gameMode) {
			pressedInUI = true;
		}
	} else if (!thumbstick && lastThumbstick) {
		pressedInUI = false;
	}
	lastThumbstick = thumbstick;

	return pressedInUI;
#endif
}

void Host_VRMotionControls( bool zoomed, bool superzoomed, bool motionActive, vec3_t hmdAngles, vec3_t handPosition, vec3_t hmdPosition, vec3_t weaponPosition)
{
	// Get information
	static float lastLen = 0;
	static float lastSpeed = 0;
	static char lastWeapon[64];
	float hmdYaw = DEG2RAD(hmdAngles[YAW]);
	float handDX = hmdPosition[0] - handPosition[0];
	float handDY = hmdPosition[2] - handPosition[2];
	float forwardHand = handDX * sin(hmdYaw) + handDY * cos(hmdYaw);
	float weaponDX = hmdPosition[0] - weaponPosition[0];
	float weaponDY = hmdPosition[2] - weaponPosition[2];
	float weaponDZ = hmdPosition[1] - weaponPosition[1];
	float lenWeapon = fabs(weaponPosition[0]) + fabs(weaponPosition[2]);
	float forwardWeapon = weaponDX * sin(hmdYaw) + weaponDY * cos(hmdYaw);
	float speed = fabs(lastLen - lenWeapon) / (float)VR_GetRefreshRate();
	float limit = Cvar_VariableValue("vr_arm_length") * 0.1f + 0.3f;
	const char* weapon = Cvar_VariableString("vr_weapon_pivot_name");

	//Hand use action
	static bool lastUse = false;
	static const char* prefixShield = "models/shield/v_";
	bool hasDual = strcmp(weapon, "models/v_elite.mdl") == 0;
	bool hasShield = strncmp(weapon, prefixShield, strlen(prefixShield)) == 0;
	bool armStretching = Cvar_VariableValue("vr_motion_activation") > 1.5f;
	bool use = (forwardHand > limit * 1.1f) && !hasShield && !hasDual && armStretching;
	if (lastUse != use) {
		Cbuf_AddText( use ? "+use\n" : "-use\n" );
		lastUse = use;
	}
	Cvar_SetValue("vr_hand_stretch", (forwardHand > limit) && !hasShield && !hasDual ? 1 : 0);

	//Knife attack
	if ((strcmp(weapon, "models/v_knife.mdl") == 0) || (strcmp(weapon, "models/shield/v_shield_knife.mdl") == 0)) {
		static bool attackStarted = false;
		static bool lastFastAttack = false;
		static bool lastSlowAttack = false;
		if (lastSpeed > speed) {
			attackStarted = true;
		}
		bool fastAttack = attackStarted && (forwardWeapon > limit * 0.5f) && (speed > 0.0002f) && !hasShield;
		bool slowAttack = attackStarted && (forwardWeapon > limit * 0.5f) && (speed > 0.0001f) && !fastAttack;

		if (fastAttack != lastFastAttack) {
			Cvar_SetValue("vr_weapon_anim", 0);
			Cbuf_AddText( fastAttack ? "+attack2\n" : "-attack2\n" );
			lastFastAttack = fastAttack;
		}
		if (slowAttack != lastSlowAttack) {
			Cvar_SetValue("vr_weapon_anim", 0);
			Cbuf_AddText( slowAttack ? "+attack\n" : "-attack\n" );
			lastSlowAttack = slowAttack;
		}
		if (fastAttack || slowAttack) {
			Cvar_LazySet("vr_weapon_anim", 0);
		} else {
			Cvar_LazySet("vr_weapon_anim", 1);
			attackStarted = false;
		}
		Cvar_LazySet("vr_weapon_throw_active", 0);
	}
	//Grenade throwing
	else if ((strcmp(weapon, "models/v_flashbang.mdl") == 0) ||
			 (strcmp(weapon, "models/v_hegrenade.mdl") == 0) ||
			 (strcmp(weapon, "models/v_smokegrenade.mdl") == 0) ||
			 (strcmp(weapon, "models/shield/v_shield_flashbang.mdl") == 0) ||
			 (strcmp(weapon, "models/shield/v_shield_hegrenade.mdl") == 0) ||
			 (strcmp(weapon, "models/shield/v_shield_smokegrenade.mdl") == 0)) {

		static float startDX = 0;
		static float startDY = 0;
		static float startDZ = 0;
		float dirX = weaponDX - startDX;
		float dirY = weaponDY - startDY;
		float dirZ = weaponDZ - startDZ;

		static bool lastThrowing = false;
		bool throwing = speed > 0.00035f;
		if (throwing) {
			float dir = sqrt(dirX * dirX + dirY * dirY);
			Cvar_LazySet("vr_weapon_throw_active", 1);
			Cvar_SetValue("vr_weapon_throw_pitch", RAD2DEG(sin(dirZ / dir)));
			Cvar_SetValue("vr_weapon_throw_yaw", RAD2DEG(atan2(dirX, dirY)));
		} else if (speed < 0.0001f) {
			Cvar_LazySet("vr_weapon_throw_active", 0);
			startDX = weaponDX;
			startDY = weaponDY;
			startDZ = weaponDZ;
		}
		if (lastThrowing) {
			float diff = hmdAngles[YAW] - Cvar_VariableValue("vr_weapon_throw_yaw");
			while (diff > 180) diff -= 360;
			while (diff <-180) diff += 360;
			//do not release the grenade when moving hand behind
			if (fabs(diff) > 100) {
				Cvar_LazySet("vr_weapon_throw_active", 0);
				throwing = true;
			}
		}
		if (throwing != lastThrowing) {
			Cbuf_AddText( throwing ? "+attack\n" : "-attack\n" );
			if (!throwing) {
				Cvar_LazySet("vr_weapon_anim", 0);
			}
			lastThrowing = throwing;
		}
	}
	//Weapon zoom by motion
	else if ((strcmp(weapon, "models/v_aug.mdl") == 0) ||
			(strcmp(weapon, "models/v_awp.mdl") == 0) ||
			(strcmp(weapon, "models/v_g3sg1.mdl") == 0) ||
			(strcmp(weapon, "models/v_scout.mdl") == 0) ||
			(strcmp(weapon, "models/v_sg550.mdl") == 0) ||
			(strcmp(weapon, "models/v_sg552.mdl") == 0)) {
		if (strcmp(weapon, lastWeapon) != 0) {
			Cvar_SetValue("vr_zoom_by_motion", 0);
		}
		static bool zoomRequested = false;
		if (zoomRequested) {
			Cbuf_AddText( "-attack2\n" );
			zoomRequested = false;
		} else if (motionActive && (!zoomed || superzoomed)) {
			Cbuf_AddText( "+attack2\n" );
			Cvar_SetValue("vr_zoom_by_motion", 1);
			zoomRequested = true;
		} else if (!motionActive && zoomed && (Cvar_VariableValue("vr_zoom_by_motion") > 0.5f)) {
			Cbuf_AddText( "+attack2\n" );
			zoomRequested = true;
		}
		Cvar_LazySet("vr_weapon_anim", 1);
		Cvar_LazySet("vr_weapon_throw_active", 0);
	} else {
		Cvar_LazySet("vr_weapon_anim", 1);
		Cvar_LazySet("vr_weapon_throw_active", 0);
	}

	//Remember last status
	lastLen = lenWeapon;
	lastSpeed = speed;
	strcpy(lastWeapon, weapon);
}

void Host_VRMovementEntity( bool zoomed, vec3_t handPosition, vec3_t hmdAngles, vec3_t hmdPosition, vec3_t weaponPosition )
{
	// Camera pushback
	float limit = 1.0f;
	float dx = hmdPosition[0] + vr_hmd_offset[0];
	float dy = hmdPosition[2] + vr_hmd_offset[1];
	if (dx > limit) { vr_hmd_offset[0] += limit - dx; dx = limit; }
	if (dx < -limit) { vr_hmd_offset[0] += -limit - dx; dx = -limit; }
	if (dy > limit) { vr_hmd_offset[1] += limit - dy; dy = limit; }
	if (dy < -limit) { vr_hmd_offset[1] += -limit - dy; dy = -limit; }

	// Camera movement
	float hmdYaw = DEG2RAD(hmdAngles[YAW]);
	float camerax = dx * cos(hmdYaw) - dy * sin(hmdYaw);
	float cameray = dx * sin(hmdYaw) + dy * cos(hmdYaw);
	float scale = Cvar_VariableValue("vr_worldscale");
	Cvar_SetValue("vr_camera_x", zoomed ? 0 : camerax * scale);
	Cvar_SetValue("vr_camera_y", zoomed ? 0 : cameray * scale);
	Cvar_SetValue("vr_camera_z", zoomed ? 0 : (hmdPosition[1] - vr_hmd_offset[2]) * scale);

	// Hand movement
	// The preserved client viewmodel code steps the model by -right*dx
	// (StudioModelRenderer), i.e. it consumes vr_hand_x/vr_weapon_x as
	// left-positive offsets. The desktop OpenXR input layer (VrInput.c)
	// delivers raw poses where +x is right, so negate the rotated X here
	// to restore the tracking contract; Y (forward) and Z (up) already match.
	dx = handPosition[0] - hmdPosition[0];
	dy = handPosition[2] - hmdPosition[2];
	float handX = -(dx * cos(hmdYaw) - dy * sin(hmdYaw));
	float handY = dx * sin(hmdYaw) + dy * cos(hmdYaw);
	Cvar_SetValue("vr_hand_x", zoomed ? INT_MAX : handX * scale);
	Cvar_SetValue("vr_hand_y", zoomed ? INT_MAX : handY * scale);
	Cvar_SetValue("vr_hand_z", zoomed ? INT_MAX : (handPosition[1] - hmdPosition[1]) * scale);

	// Weapon movement
	// Same left-positive X contract as the hand offset above.
	dx = weaponPosition[0] - hmdPosition[0];
	dy = weaponPosition[2] - hmdPosition[2];
	float weaponX = -(dx * cos(hmdYaw) - dy * sin(hmdYaw));
	float weaponY = dx * sin(hmdYaw) + dy * cos(hmdYaw);
	Cvar_SetValue("vr_weapon_x", zoomed ? INT_MAX : weaponX * scale);
	Cvar_SetValue("vr_weapon_y", zoomed ? INT_MAX : weaponY * scale);
	Cvar_SetValue("vr_weapon_z", zoomed ? INT_MAX : (weaponPosition[1] - vr_hmd_offset[2]) * scale);
}

void Host_VRMovementPlayer( vec3_t hmdAngles, vec3_t hmdPosition, vec3_t weaponAngles, float thumbstickX, float thumbstickY )
{
	// Recenter if player position changed way too much
	bool reset = false;
	vec3_t currentPosition;
	static vec3_t lastPosition = {};
	float scale = Cvar_VariableValue("vr_worldscale");
	currentPosition[0] = Cvar_VariableValue("vr_player_pos_x");
	currentPosition[1] = Cvar_VariableValue("vr_player_pos_y");
	currentPosition[2] = Cvar_VariableValue("vr_player_pos_z");
	if (VectorDistance(currentPosition, lastPosition) > scale) {
		Cvar_SetValue("vr_vignette_amount", 1);
#ifdef ANDROID
		VR_SetConfig(VR_CONFIG_NEED_RECENTER, true);
#else
		/* A server teleport must not rotate the OpenXR reference space. */
		vr_hmd_offset[0] = -hmdPosition[0];
		vr_hmd_offset[1] = -hmdPosition[2];
#endif
		reset = true;
	}

	// Comfort vignette
	if (Cvar_VariableValue("vr_vignette") > 0.5f) {
		bool increase = VectorDistance(currentPosition, lastPosition) > scale * 0.1f;
		float amount = Cvar_VariableValue("vr_vignette_amount");
		if (increase)
			amount = (amount + 1.0f) / 2.0f;
		else if (amount > 0)
			amount -= 0.005f;
		else
			amount = 0;
		Cvar_SetValue("vr_vignette_amount", amount);
	}

	// Reset offset if OS recenter was called
	if (VR_DidRecenter()) {
		Cvar_SetValue("vr_vignette_amount", 1);
		vr_hmd_offset[0] = 0;
		vr_hmd_offset[1] = 0;
		vr_hmd_offset[2] = hmdPosition[1];
		reset = true;
	}

	// Player movement
	bool move6DoF = false;
	static bool lastMove6Dof = false;
	float hmdYaw = DEG2RAD(hmdAngles[YAW]);
	float deadzone = Cvar_VariableValue("vr_thumbstick_deadzone_left");
	if ((fabs(thumbstickX) < deadzone) && (fabs(thumbstickY) < deadzone)) {
		thumbstickX = 0;
		thumbstickY = 0;
		if ((Cvar_VariableValue("vr_6dof") > 0) && (Cvar_VariableValue("vr_spectator") < 0.5f)) {
			float movementScale = 0.2f;   // How much should the movement be mapped to joystick
			float minimalMovement = 0.3f; // Filter small movements to not spam the server

			// Compensate movement from the previous frame
			if (lastMove6Dof && !reset) {
				vec2_t hmdPos = {-hmdPosition[0], -hmdPosition[2]};
				vec2_t playerDiff = {(currentPosition[0] - lastPosition[0]) / scale, (currentPosition[1] - lastPosition[1]) / scale};
				float lerp = sqrt(powf(playerDiff[0], 2.0f) + powf(playerDiff[1], 2.0f));
				Vector2Lerp(vr_hmd_offset, lerp > 1 ? 1 : lerp, hmdPos, vr_hmd_offset);
			}

			// Send movement using joystick interface
			float yaw = hmdYaw - DEG2RAD(weaponAngles[YAW]);
			float dx = Cvar_VariableValue("vr_camera_x") / scale;
			float dy = -Cvar_VariableValue("vr_camera_y") / scale;
			if (fabs(dx) + fabs(dy) > minimalMovement) {
				thumbstickX = dx * cos(yaw) - dy * sin(yaw);
				thumbstickY = dx * sin(yaw) + dy * cos(yaw);
				thumbstickX *= movementScale;
				thumbstickY *= movementScale;
				move6DoF = true;
			}
		}
	} else if (Cvar_VariableValue("vr_spectator") < 0.5f) {
		float yaw = 0;
		float dx = thumbstickX;
		float dy = thumbstickY;

		// Detect ladder direction
		static bool wasOnLadder = false;
		static float enterLadderYaw = 0;
		bool isOnLadder = Cvar_VariableValue("vr_ladder") > 0.5f;
		if (isOnLadder && !wasOnLadder) {
			enterLadderYaw = DEG2RAD(weaponAngles[YAW]);
		}
		wasOnLadder = isOnLadder;

		// Limit ladder movement
		if (isOnLadder) {
			float diff = enterLadderYaw - DEG2RAD(weaponAngles[YAW]);
			while (diff > M_PI) diff -= M_PI2;
			while (diff <-M_PI) diff += M_PI2;
			if (fabs(diff) > DEG2RAD(15)) {
				dx = 0;
				dy = 0;
			} else if (!vr_ducked) {
				dx = 0;
			}
		}

		// Apply walk direction
		else if (Cvar_VariableValue("vr_walkdirection") > 0.5f) {
			yaw = hmdYaw - DEG2RAD(weaponAngles[YAW]);
		}
		thumbstickX = dx * cos(yaw) - dy * sin(yaw);
		thumbstickY = dx * sin(yaw) + dy * cos(yaw);
	}

	// Update client
	if (!cl.paused) clgame.dllFuncs.pfnMoveEvent( thumbstickY, thumbstickX );
	VectorCopy(currentPosition, lastPosition);
	lastMove6Dof = move6DoF;
}

void Host_VRRotations( bool zoomed, vec3_t handAngles, vec3_t hmdAngles, vec3_t hmdPosition, vec3_t weaponAngles, float thumbstickX, float thumbstickY )
{
#ifndef ANDROID
	{
	/* Keep tracking-to-world heading independent of render passes, recoil and
	 * controller angle wrap. The client consumes relative aim once per frame. */
	if (!vr_tracking_anchored) {
		vr_tracking_yaw = cl.viewangles[YAW] - weaponAngles[YAW];
		vr_tracking_anchored = true;
	}
	static bool turnHeld;
	bool turn = fabsf(thumbstickX) > 0.5f;
	bool smooth = Cvar_VariableValue("vr_turn_type") > 0.5f;
	if (turn && (smooth || !turnHeld)) {
		float step = Cvar_VariableValue("vr_turn_angle");
		if (smooth) step *= 2.0f * (float)host.frametime;
		vr_tracking_yaw += thumbstickX > 0 ? -step : step;
	}
	turnHeld = turn;
	float yaw = remainderf(vr_tracking_yaw + weaponAngles[YAW] - cl.viewangles[YAW], 360.0f);
	float pitch = weaponAngles[PITCH] - cl.viewangles[PITCH];
	if (!cl.paused) clgame.dllFuncs.pfnLookEvent(yaw, pitch);
	Cvar_SetValue("vr_weapon_roll", weaponAngles[ROLL]);
	Cvar_SetValue("vr_hand_pitch", handAngles[PITCH]);
	Cvar_SetValue("vr_hand_yaw", remainderf(handAngles[YAW] - weaponAngles[YAW], 360.0f));
	Cvar_SetValue("vr_hand_roll", handAngles[ROLL]);
	Cvar_SetValue("vr_hmd_pitch", hmdAngles[PITCH]);
	Cvar_SetValue("vr_hmd_yaw", hmdAngles[YAW] + vr_tracking_yaw);
	Cvar_SetValue("vr_hmd_roll", hmdAngles[ROLL]);
	static bool changeHeld;
	bool change = fabsf(thumbstickY) > 0.5f;
	if (change && !changeHeld) Cbuf_AddText(thumbstickY > 0 ? "invnext\n" : "invprev\n");
	changeHeld = change;
	return;
	}
#endif
	// Weapon rotation
	static float lastYaw = 0;
	static float lastPitch = 0;
	float yaw = weaponAngles[YAW] - lastYaw;
	float pitch = weaponAngles[PITCH] - lastPitch;
	float diff = lastPitch - Cvar_VariableValue("vr_player_pitch");
	if ((fabs(diff) > 1) && !zoomed) {
		pitch += diff + 0.02f;
	}
	lastYaw = weaponAngles[YAW];
	lastPitch = weaponAngles[PITCH];
	Cvar_SetValue("vr_weapon_roll", weaponAngles[ROLL]);

	// Hand rotation
	Cvar_SetValue("vr_hand_pitch", handAngles[PITCH]);
	Cvar_SetValue("vr_hand_yaw", handAngles[YAW] - weaponAngles[YAW]);
	Cvar_SetValue("vr_hand_roll", handAngles[ROLL]);

	// Snap turn
	float snapTurnStep = 0;
	bool spectateFPV = fabs(Cvar_VariableValue("vr_spectator") - 4) < 0.1f;
	bool smoothTurn = Cvar_VariableValue("vr_turn_type") > 0.5f;
	bool snapTurnDown = fabs(thumbstickX) > 0.5f;
	static bool lastSnapTurnDown = false;
	if (!spectateFPV && snapTurnDown && (smoothTurn || !lastSnapTurnDown)) {
		float angle = Cvar_VariableValue("vr_turn_angle");
		if (smoothTurn) {
			angle *= 0.02f;
		}
		snapTurnStep = thumbstickX > 0 ? -angle : angle;
		Cvar_SetValue("vr_vignette_amount", 1);
		vr_hmd_offset[0] = -hmdPosition[0];
		vr_hmd_offset[1] = -hmdPosition[2];
		yaw += snapTurnStep;
	}
	lastSnapTurnDown = snapTurnDown;
	clgame.dllFuncs.pfnLookEvent( yaw, pitch );

	// Weapon changing
	bool weaponChangeDown = fabs(thumbstickY) > 0.5f;
	static bool lastWeaponChangeDown = false;
	if (weaponChangeDown && !lastWeaponChangeDown) {
		Cbuf_AddText( thumbstickY > 0 ? "invnext\n" : "invprev\n" );
		Cbuf_AddText( "+attack\n" );
	} else if (!weaponChangeDown && lastWeaponChangeDown) {
		Cbuf_AddText( "-attack\n" );
	}
	lastWeaponChangeDown = weaponChangeDown;

	// HMD view
	static float lastWeaponYaw = 0;
	hmdAngles[YAW] += Cvar_VariableValue("vr_player_yaw") - lastWeaponYaw;
	Cvar_SetValue("vr_hmd_pitch", hmdAngles[PITCH]);
	Cvar_SetValue("vr_hmd_yaw", hmdAngles[YAW] + snapTurnStep);
	Cvar_SetValue("vr_hmd_roll", hmdAngles[ROLL]);
	lastWeaponYaw = weaponAngles[YAW];
}

bool Host_VRWeaponCalibration( float thumbstickX, float thumbstickY )
{
	if (Cvar_VariableValue("vr_weapon_calibration_on") > 0) {
		// Get selected CVAR
		static int axis = 0;
		static char cvar[64];
		switch (axis) {
			case 0: sprintf(cvar, "%s", "vr_weapon_pivot_x"); break;
			case 1: sprintf(cvar, "%s", "vr_weapon_pivot_y"); break;
			case 2: sprintf(cvar, "%s", "vr_weapon_pivot_z"); break;
			case 3: sprintf(cvar, "%s", "vr_weapon_pivot_pitch"); break;
			case 4: sprintf(cvar, "%s", "vr_weapon_pivot_yaw"); break;
			case 5: sprintf(cvar, "%s", "vr_weapon_pivot_scale"); break;
		}

		// Write info on the screen
		static char text[256];
		float value = Cvar_VariableValue(cvar);
		sprintf(text, "%s: %.1f", cvar, value);
		CL_CenterPrint(text, 0.5f);

		// Changing axis
		float deadzone = Cvar_VariableValue("vr_thumbstick_deadzone_right");
		bool changeAxisDown = fabs(thumbstickX) > deadzone;
		static bool lastChangeAxisDown = false;
		if (changeAxisDown && !lastChangeAxisDown) {
			axis += thumbstickX > 0 ? 1 : -1;
			if (axis < 0) axis = 0;
			if (axis > 5) axis = 5;
		}
		lastChangeAxisDown = changeAxisDown;

		// Changing value
		bool changeValueDown = fabs(thumbstickY) > deadzone;
		static bool lastChangeValueDown = false;
		if (changeValueDown && !lastChangeValueDown) {
			value += thumbstickY > 0 ? 0.5f : -0.5f;
			Cvar_SetValue(cvar, value);
			Cvar_SetValue("vr_weapon_calibration_update", 1);
		}
		lastChangeValueDown = changeValueDown;
		return true;
	}
	return false;
}

void Host_VRWeaponCrosshair( bool zoomed )
{
	// Get player position and direction
	vec3_t vecSrc, vecDir, vecEnd;
	vecDir[0] = Cvar_VariableValue("vr_player_dir_x");
	vecDir[1] = Cvar_VariableValue("vr_player_dir_y");
	vecDir[2] = Cvar_VariableValue("vr_player_dir_z");
	vecSrc[0] = Cvar_VariableValue("vr_player_pos_x");
	vecSrc[1] = Cvar_VariableValue("vr_player_pos_y");
	vecSrc[2] = Cvar_VariableValue("vr_player_pos_z");

	// Set cross-hair position far away
	for (int j = 0; j < 3; j++) {
		vecEnd[j] = vecSrc[j] + 4096.0f * vecDir[j];
	}

	// Test if there is a closer surface cross-hair should point to
	pmtrace_t trace = CL_TraceLine( vecSrc, vecEnd, PM_STUDIO_IGNORE );
	if( trace.fraction != 1.0f ) {
		for (int j = 0; j < 3; j++) {
			vecEnd[j] = trace.endpos[j];
		}
	}

	// Decide if crosshair should be visible
	bool visible = !zoomed;
	const char* weapon = Cvar_VariableString("vr_weapon_pivot_name");
	if ((strcmp(weapon, "models/v_knife.mdl") == 0) ||
		(strcmp(weapon, "models/v_flashbang.mdl") == 0) ||
		(strcmp(weapon, "models/v_hegrenade.mdl") == 0) ||
		(strcmp(weapon, "models/v_smokegrenade.mdl") == 0) ||
		(strcmp(weapon, "models/shield/v_shield_knife.mdl") == 0) ||
		(strcmp(weapon, "models/shield/v_shield_flashbang.mdl") == 0) ||
		(strcmp(weapon, "models/shield/v_shield_hegrenade.mdl") == 0) ||
		(strcmp(weapon, "models/shield/v_shield_smokegrenade.mdl") == 0)) {
		visible = false;
	} else if ((strcmp(weapon, "models/v_aug.mdl") == 0) || (strcmp(weapon, "models/v_sg552.mdl") == 0)) {
		visible = true;
	}

	// Convert the position into screen coordinates
	vec3_t screenPos = {};
	if (!zoomed) {
		TriWorldToScreen(vecEnd, screenPos);
	}
#ifndef ANDROID
	{
		float eye[3]={Cvar_VariableValue("vr_eye_x"), Cvar_VariableValue("vr_eye_y"), Cvar_VariableValue("vr_eye_z")};
		float q[4]={Cvar_VariableValue("vr_eye_qx"), Cvar_VariableValue("vr_eye_qy"), Cvar_VariableValue("vr_eye_qz"), Cvar_VariableValue("vr_eye_qw")};
		float fov[4]={Cvar_VariableValue("vr_eye_left"), Cvar_VariableValue("vr_eye_right"), Cvar_VariableValue("vr_eye_down"), Cvar_VariableValue("vr_eye_up")};
		float matrix[4][4], px, py;
		XRUI_Projection(matrix, refState.width, refState.height,
			Q_max(0.25f,Cvar_VariableValue("vr_hud_distance")), Q_max(0.1f,Cvar_VariableValue("vr_hud_width")), eye, q, fov);
		if (XRUI_PixelFromNDC(matrix, screenPos[0], screenPos[1], &px, &py)) {
			screenPos[0]=2*px/refState.width-1;
			screenPos[1]=1-2*py/refState.height;
		} else visible=false;
	}
#endif
	Cvar_SetValue("vr_xhair_x", visible ? screenPos[0] : INT_MAX);
	Cvar_SetValue("vr_xhair_y", visible ? screenPos[1] : INT_MAX);
}
