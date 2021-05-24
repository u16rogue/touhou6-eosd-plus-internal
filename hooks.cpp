#include "hooks.h"
#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include "utils.h"
#include "global.h"
#include "key_remap_config.h"

// ==============
// function hooks
// ==============

HRESULT (__stdcall*o_GetDeviceState)(void *, DWORD, LPVOID) = nullptr;
HRESULT __stdcall hk_GetDeviceState(void *thisptr, DWORD cbData, LPVOID lpvData)
{
	HRESULT result = o_GetDeviceState(thisptr, cbData, lpvData);

	std::uint8_t *datau8 = reinterpret_cast<std::uint8_t *>(lpvData);

	if (datau8[TOGGLE_ORIGINAL_DISABLE] & 0x80)
	{
		global::disable_original_keys ^= true;
		MessageBoxW(nullptr, global::disable_original_keys ? L"Original keys are now disabled" : L"Original keys are now enabled", MSG_TITLE, NULL);
	}

	for (auto key_remap : remapped_keys_list)
	{
		if (datau8[key_remap.k_old] & 0x80)
		{
			datau8[key_remap.k_old] &= ~0x80;
			datau8[key_remap.k_new] |= 0x80;
		}
		else if (global::disable_original_keys && datau8[key_remap.k_new] & 0x80)
		{
			datau8[key_remap.k_new] &= ~0x80;
		}
	}

	return result;
}

// ==============
// initialization
// ==============

bool hooks::init_hooks()
{
	if (!utils::vtable_hook(global::lpdirectinput[0], 9, hk_GetDeviceState, reinterpret_cast<void **>(&o_GetDeviceState)))
	{
		MessageBoxW(nullptr, L"Failed to hook GetDeviceState", MSG_TITLE, 0);
		return false;
	}

	return true;
}