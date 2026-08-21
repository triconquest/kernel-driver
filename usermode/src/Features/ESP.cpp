#include "ESP.h"
#include "../Kernel/Driver.h"
#include "../Utils/Offsets.h"
#include "../Menu/ImGui/imgui.h"

#include <algorithm>
#include <cstdio>

std::vector<PlayerData> CESP::GetPlayers(uintptr_t moduleBase, int localTeam)
{
	std::vector<PlayerData> players;

	uintptr_t localPlayerPtr = Driver::Read<uintptr_t>(m_hDriver, moduleBase + Offsets::LocalPlayer);

	if (!localPlayerPtr)
		return players;

	Vector3 localOrigin = Driver::Read<Vector3>(m_hDriver, localPlayerPtr + Offsets::m_vecAbsOrigin);

	uintptr_t entListBase = Driver::Read<uintptr_t>(m_hDriver, moduleBase + Offsets::EntityList);

	if (!entListBase)
		return players;

	for (int i = 0; i < Offsets::MaxEntities; ++i) {
		uintptr_t entPtr = Driver::Read<uintptr_t>(m_hDriver, entListBase + i * Offsets::EntityListEntrySize);

		if (!entPtr || entPtr == localPlayerPtr)
			continue;

		int lifeState = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_lifeState);

		if (lifeState != LIFE_ALIVE && lifeState != LIFE_DYING)
			continue;

		int team = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iTeamNum);

		if (localTeam != -1 && team == localTeam)
			continue;

		PlayerData pd = {};
		pd.entityPtr = entPtr;
		pd.team = team;
		pd.lifeState = lifeState;
		pd.isKnocked = (lifeState == LIFE_DYING);
		pd.isZooming = Driver::Read<bool>(m_hDriver, entPtr + Offsets::m_bZooming);

		pd.origin = Driver::Read<Vector3>(m_hDriver, entPtr + Offsets::m_vecAbsOrigin);

		Vector3 viewOffset = Driver::Read<Vector3>(m_hDriver, entPtr + Offsets::m_vecViewOffset);
		pd.headPos = { pd.origin.x + viewOffset.x, pd.origin.y + viewOffset.y, pd.origin.z + viewOffset.z };

		pd.health = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iHealth);
		pd.maxHealth = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_iMaxHealth);
		pd.shield = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_shieldHealth);
		pd.maxShield = Driver::Read<int>(m_hDriver, entPtr + Offsets::m_shieldHealthMax);

		pd.distance = GetDistance(localOrigin, pd.origin);
		pd.valid = true;

		players.push_back(pd);
	}

	return players;
}

void CESP::Run(uintptr_t moduleBase)
{
	uintptr_t localPlayerPtr = Driver::Read<uintptr_t>(m_hDriver, moduleBase + Offsets::LocalPlayer);

	if (!localPlayerPtr)
		return;

	int localTeam = Driver::Read<int>(m_hDriver, localPlayerPtr + Offsets::m_iTeamNum);

	std::vector<PlayerData> players = GetPlayers(moduleBase, localTeam);

	if (players.empty())
		return;

	Matrix4 viewMatrix = Driver::Read<Matrix4>(m_hDriver, moduleBase + Offsets::ViewMatrix);

	ImGuiIO& io = ImGui::GetIO();
	int screenW = static_cast<int>(io.DisplaySize.x);
	int screenH = static_cast<int>(io.DisplaySize.y);

	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	for (const PlayerData& player : players) {
		if (!player.valid)
			continue;

		Vector2 headScreen, feetScreen;

		if (!WorldToScreen(player.headPos, headScreen, viewMatrix, screenW, screenH))
			continue;

		if (!WorldToScreen(player.origin, feetScreen, viewMatrix, screenW, screenH))
			continue;

		float boxHeight = feetScreen.y - headScreen.y;

		if (boxHeight <= 0.f)
			continue;

		float boxWidth = boxHeight * 0.55f;
		float boxX = headScreen.x - boxWidth * 0.5f;
		float boxY = headScreen.y;

		ImU32 boxColor = player.isKnocked ? IM_COL32(255, 200, 0, 255) : IM_COL32(255, 60, 60, 255);

		drawList->AddRect(ImVec2(boxX, boxY), ImVec2(boxX + boxWidth, boxY + boxHeight), boxColor, 0.f, 0, 1.5f);

		if (player.maxHealth > 0) {
			float healthPct = std::clamp(static_cast<float>(player.health) / static_cast<float>(player.maxHealth), 0.f, 1.f);
			float barHeight = boxHeight * healthPct;

			drawList->AddRectFilled(
				ImVec2(boxX - 6.f, boxY + boxHeight - barHeight),
				ImVec2(boxX - 3.f, boxY + boxHeight),
				IM_COL32(0, 220, 0, 255));
		}

		char distanceLabel[32];
		snprintf(distanceLabel, sizeof(distanceLabel), "%.0fm", player.distance);
		drawList->AddText(ImVec2(boxX, boxY - 14.f), IM_COL32(255, 255, 255, 255), distanceLabel);
	}
}
