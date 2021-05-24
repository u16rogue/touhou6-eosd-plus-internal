#pragma once

#include <cstddef>
#include <cstdint>

namespace utils
{
	bool iat_hook(const char *module_name, const char *api_name, void *hook_ptr, void **orig_ptrptr);
	bool vtable_hook(void **vtable, int index, void *hook_ptr, void **orig_ptrptr);
	std::uint8_t *aob_scan(void *start, std::size_t size, const char *signature, const char *mask);
}

#define make_vtable_hook(classinst_ptr, vtableindex, func_index, func) \
	utils::vtable_hook(reinterpret_cast<void***>(classinst_ptr)[vtableindex], func_index, &hk_##func##, reinterpret_cast<void **>(&o_##func##))

#define make_mod_iat_hook(modname, func) \
	utils::iat_hook(modname, ## #func ##, &hk_##func##, reinterpret_cast<void **>(&o_##func##))

#define make_any_iat_hook(func) \
	make_mod_iat_hook(nullptr, func)