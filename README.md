# Touhou 6 - EOSD Internal
Remaps Touhou 6 key binds internally by hooking **IDirectInputDevice8::GetDeviceState** and modifying **lpvData**.

## Key remaps

| Old Key | New Key |
|:-------:|:-------:|
|    Z    |  SPACE  |
|    X    |    E    |
|    ↑    |    W    |
|    ←    |    A    |
|    ↓    |    S    |
|    →    |    D    |

* Special key <kbd>INSERT</kbd> - Toggle to enable / disable the original keys

## Usage
* Inject `th6eosdplusinternal.dll` to `東方紅魔郷.exe`

## Custom Keybinds
* The only way to change the remapped keybinds is by editing [key_remap_config.h](key_remap_config.h) and then recompiling.
* All keys are defined in [dinput.h](include/dinput.h) which TH6's engine uses.

## Technical stuff
*Information is from v1.02*
* Pointer to the device input can be found at `8B 15 ? ? ? ? 8B 02 8B 0D ? ? ? ? 51 FF 50 24 89 85` @ `東方紅魔郷.exe`

    ```asm
    8B 15 28 6D 6C 00  mov     edx, dword_6C6D28
    8B 02              mov     eax, [edx]
    8B 0D 28 6D 6C 00  mov     ecx, dword_6C6D28
    51                 push    ecx
    FF 50 24           call    dword ptr [eax+24h]
    ```
    * `eax + 24h` indicates a call to the 9th index at the first vtable contained at 0x6C6D28

* vpatch creates a jump patch before the call reaches this region, hopefully it shouldnt interfere with vpatch and other patches since this is just a vtable hook. i didn't double check if other patches also modify some parts of dinput (although im aware that thcrap does modify direct 8) so just a heads up.
* for some reason dummy devices dont work well, perhaps direct device input instances doesn't share the same device pointer which causes this project to rely on signature scanning instead.