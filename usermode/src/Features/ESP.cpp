//#include "ESP.h"
//#include "../Kernel/Driver.h"
//#include "../Utils/Offsets.h"
//
//inline void CESP::Run(uintptr_t moduleBase, int localTeam)
//{
//
//}
//
//inline std::vector<PlayerData> CESP::GetPlayers(uintptr_t moduleBase, int localTeam)
//{
//	std::vector<PlayerData> players;
//
//	uintptr_t localPlayerPtr = Driver::Read<uintptr_t>(m_hDriver, moduleBase + Offsets::LocalPlayer);
//
//	if (!localPlayerPtr)
//		return players;
//
//	Vector3 localOrigin = Driver::Read<Vector3>(m_hDriver, localPlayerPtr + Offsets::m_vecAbsOrigin);
//
//	uintptr_t entListBase = Driver::Read<uintptr_t>(m_hDriver, moduleBase + Offsets::EntityList);
//
//	if (!entListBase)
//		return players;
//
//	for (int i = 0; i < 60; ++i) {
//		uintptr_t entPtr = Driver::Read<uintptr_t>(m_hDriver, entListBase + i * Offsets::entitySize);// i dont have offset for entitySize?
//
//		if (!entPtr || entPtr == localPlayerPtr)
//			continue;
//
//		int lifeState = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_lifeState);
//
//		if (lifeState != LIFE_ALIVE && lifeState != LIFE_DYING)
//			continue;
//
//		int team = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iTeamNum);
//
//		if (localTeam != -1 && team == localTeam)
//			continue;
//
//		PlayerData pd = {};
//		pd.entityPtr = entPtr;
//		pd.team = team;
//		pd.lifeState = lifeState;
//		pd.isKnocked = (lifeState == LIFE_DYING);
//		pd.isZooming = Driver::Read<bool>(m_hDriver, entPtr + Offsets::m_bZooming);
//
//		pd.origin = Driver::Read<Vector3>(m_hDriver, entPtr + Offsets::m_vecAbsOrigin);
//
//		pd.headPos = { pd.origin.x, pd.origin.y, pd.origin.z + 72.f }; // hardcoded for now, TODO: use bone matrix
//		pd.health = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iHealth);
//		pd.maxHealth = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iMaxHealth);
//		pd.shield = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_shieldHealth);
//		pd.maxShield = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_shieldHealthMax); // whats the difference between extraShieldHealth vs shieldHealth?
//
//		pd.valid = true;
//
//		players.push_back(pd);
//	}
//
//	return players;
//}
