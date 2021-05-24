#pragma once

#include "include/dinput.h"

// ignore this, this is for the internal flags
enum th_ctrl_flag
{
	OK     = 0x00000001,
	CANCEL = 0x00000002,
	PAUSE  = 0x00000008,
	UP     = 0x00000010,
	DOWN   = 0x00000020,
	LEFT   = 0x00000040,
	RIGHT  = 0x00000080
};

struct key_remap
{
	key_remap(int k_old_, int k_new_)
		: k_old(k_old_), k_new(k_new_)
	{}

	const int k_old;
	const int k_new;
};

// To define custom key remaps simple add a new key_remap struct into the array
// Syntax: key_remap(<key input from user>, <key the game will interpret>)

inline static key_remap remapped_keys_list[] =
{
	key_remap(DIK_W, DIK_UPARROW),
	key_remap(DIK_A, DIK_LEFTARROW),
	key_remap(DIK_S, DIK_DOWNARROW),
	key_remap(DIK_D, DIK_RIGHTARROW),
	key_remap(DIK_SPACE, DIK_Z),
	key_remap(DIK_E, DIK_X)
};

#define TOGGLE_ORIGINAL_DISABLE DIK_INSERT