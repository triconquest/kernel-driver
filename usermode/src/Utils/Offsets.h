#pragma once

#include <cstdint>

namespace Offsets {
	constexpr uintptr_t EntityList = 0x585b4d8;
	constexpr uintptr_t LocalPlayer = 0x26781f8;
	constexpr uintptr_t ViewMatrix = 0x11a390;

	constexpr uintptr_t EntityListEntrySize = 0x20;
	constexpr int MaxEntities = 64;

	constexpr uintptr_t m_vecAbsOrigin = 0x16c;
	constexpr uintptr_t m_iTeamNum = 0x334;

	constexpr uintptr_t m_iHealth = 0x324;
	constexpr uintptr_t m_iMaxHealth = 0x470;
	constexpr uintptr_t m_lifeState = 0x698;
	constexpr uintptr_t m_bZooming = 0x1cc1;

	constexpr uintptr_t m_shieldHealth = 0x190;
	constexpr uintptr_t m_shieldHealthMax = 0x194;
	constexpr uintptr_t m_vecViewOffset = 0x48;
}
