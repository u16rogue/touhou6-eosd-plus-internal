#include <Windows.h>
#include "global.h"
#include "hooks.h"
#include "utils.h"
#include <Psapi.h>
#include "include/dinput.h"

bool main_init()
{
    MODULEINFO mi = {};
    if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandleW(nullptr), &mi, sizeof(mi)))
    {
        MessageBoxW(nullptr, L"Failed to obtain main module", MSG_TITLE, 0);
        return false;
    }
    auto sig_res_lpdxinput = utils::aob_scan(mi.lpBaseOfDll, mi.SizeOfImage, "\x8B\x15\x00\x00\x00\x00\x8B\x02\x8B\x0D\x00\x00\x00\x00\x51\xFF\x50\x24\x89\x85", "xx????xxxx????xxxxxx");
    if (!sig_res_lpdxinput)
    {
        MessageBoxW(nullptr, L"Failed to find DirectInput signature", MSG_TITLE, 0);
        return false;
    }

    global::lpdirectinput = **reinterpret_cast<void *****>(sig_res_lpdxinput + 0x2);
    return true;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (HANDLE init_thread = nullptr; ul_reason_for_call == DLL_PROCESS_ATTACH && (init_thread = CreateThread(nullptr, NULL, [](LPVOID arg0) -> DWORD
    {
        HMODULE mod = reinterpret_cast<HMODULE>(arg0);

        DisableThreadLibraryCalls(mod);

        if (!main_init() || !hooks::init_hooks())
            FreeLibraryAndExitThread(mod, 0);

        MessageBoxW(nullptr, L"Successfully initialized!\n\nPress <insert> to enable/disable original keys.", MSG_TITLE, 0);

        return 0;
    }, hModule, NULL, nullptr))) { CloseHandle(init_thread); }
    
    return TRUE;
}

