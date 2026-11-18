#pragma once
#include <Windows.h>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <string>

// Settings that control the loader UI. They are read once from ModLoaderInfo.ini
// during profile setup and are intentionally not exported, so mods built against
// older headers keep working.
namespace UISettings
{
	// Virtual key that opens and closes the loader menu.
	// Overridden by [UI] MenuKey in ModLoaderInfo.ini.
	inline int MenuToggleKey = VK_F8;

	// Resolved name of MenuToggleKey, used for log output only.
	inline std::string MenuToggleKeyName = "F8";

	// Maps a key name to a virtual-key code. Accepts:
	//   F1 - F24
	//   A - Z and 0 - 9
	//   raw virtual-key codes in decimal or 0x-prefixed hex
	// Returns fallback when the name is empty or not recognized.
	inline int ParseVirtualKey(const std::string& name, int fallback = VK_F8)
	{
		std::string key;
		key.reserve(name.size());
		for (char c : name)
		{
			if (!std::isspace(static_cast<unsigned char>(c)))
				key.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
		}

		if (key.empty())
			return fallback;

		// Function keys: F1 - F24
		if (key.size() >= 2 && key[0] == 'F' && std::isdigit(static_cast<unsigned char>(key[1])))
		{
			const int number = std::atoi(key.c_str() + 1);
			if (number >= 1 && number <= 24)
				return VK_F1 + (number - 1);
			return fallback;
		}

		// Single character keys: A-Z and 0-9
		if (key.size() == 1)
		{
			const char c = key[0];
			if ((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				return c;
		}

		// Raw virtual-key code: decimal, or 0x-prefixed hex
		int base = 10;
		const char* digits = key.c_str();
		if (key.size() > 2 && key[0] == '0' && key[1] == 'X')
		{
			base = 16;
			digits += 2;
		}

		char* end = nullptr;
		const long code = std::strtol(digits, &end, base);
		if (end != digits && *end == '\0' && code > 0 && code < 0xFF)
			return static_cast<int>(code);

		return fallback;
	}

	// Human readable form of a virtual-key code, for log output only.
	inline std::string VirtualKeyName(int vk)
	{
		if (vk >= VK_F1 && vk <= VK_F24)
			return "F" + std::to_string(vk - VK_F1 + 1);
		if ((vk >= 'A' && vk <= 'Z') || (vk >= '0' && vk <= '9'))
			return std::string(1, static_cast<char>(vk));

		char buffer[16] = { 0 };
		std::snprintf(buffer, sizeof(buffer), "0x%02X", vk);
		return buffer;
	}
}
