#include "utils.h"
#include <string>
#include <Windows.h>

bool utils::iat_hook(const char *module_name, const char *api_name, void *hook_ptr, void **orig_ptrptr)
{
	static std::uint8_t *base = reinterpret_cast<std::uint8_t *>(GetModuleHandleA(module_name));
	if (!base)
		return false;

	static PIMAGE_NT_HEADERS nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>(base + reinterpret_cast<PIMAGE_DOS_HEADER>(base)->e_lfanew);
	if (!nt_header)
		return false;

	PIMAGE_IMPORT_DESCRIPTOR current_import_descriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(base + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	if (!current_import_descriptor->Characteristics)
		return false;

	do
	{
		PIMAGE_THUNK_DATA oft = reinterpret_cast<PIMAGE_THUNK_DATA>(base + current_import_descriptor->OriginalFirstThunk);
		PIMAGE_THUNK_DATA ft = reinterpret_cast<PIMAGE_THUNK_DATA>(base + current_import_descriptor->FirstThunk);

		while (oft->u1.AddressOfData)
		{
			const char *n = reinterpret_cast<const char *>(reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(base + oft->u1.AddressOfData)->Name);
			if (oft->u1.Ordinal & IMAGE_ORDINAL_FLAG || strcmp(api_name, n) != 0)
			{
				++oft;
				++ft;
				continue;
			}

			DWORD o_prot = 0;
			if (!VirtualProtect(&ft->u1.Function, sizeof(ft->u1.Function), PAGE_READWRITE, &o_prot))
				return false;

			if (orig_ptrptr)
				*orig_ptrptr = reinterpret_cast<void *>(ft->u1.Function);

			ft->u1.Function = reinterpret_cast<std::uintptr_t>(hook_ptr);

			if (!VirtualProtect(&ft->u1.Function, sizeof(ft->u1.Function), o_prot, &o_prot))
				return false;

			return true;
		}

	} while ((++current_import_descriptor)->Characteristics);

	return false;
}

bool utils::vtable_hook(void **vtable, int index, void *hook_ptr, void **orig_ptrptr)
{
	void **fn_entry = vtable + index;

	DWORD orig_prot = 0;
	if (!VirtualProtect(fn_entry, sizeof(fn_entry), PAGE_READWRITE, &orig_prot))
		return false;

	if (orig_ptrptr)
		*orig_ptrptr = vtable[index];

	vtable[index] = hook_ptr;

	if (!VirtualProtect(fn_entry, sizeof(fn_entry), orig_prot, &orig_prot))
		return false;

	return true;
}

std::uint8_t *utils::aob_scan(void *start, std::size_t size, const char *signature, const char *mask)
{
	if (!start || size < 1 || !signature || !mask)
		return nullptr;

	std::uint8_t *current_address = reinterpret_cast<std::uint8_t *>(start);
	const std::size_t  byte_count = std::strlen(mask);
	const std::uint8_t *end = current_address + size;

	do
	{
		for (std::size_t i = 0; i <= byte_count; i++)
		{
			if (mask[i] == '?')
				continue;
			else if (mask[i] != 'x' || current_address[i] != reinterpret_cast<const unsigned char *>(signature)[i])
				break;
			else if (mask[i + 1] == '\0')
				return current_address;
		}
	} 	while (++current_address + byte_count <= end);

	return nullptr;
}