#pragma once
#include "../Utils/Math.h"

#include <vector>
#include <Windows.h>

constexpr int LIFE_ALIVE = 0;
constexpr int LIFE_DYING = 1;
constexpr int LIFE_DEAD = 2;

struct PlayerData {
	uintptr_t entityPtr;
	Vector3 origin;
	Vector3 headPos;
	int health;
	int maxHealth;
	int shield;
	int maxShield;
	int team;
	int lifeState;
	bool isKnocked;
	bool isZooming;
	float distance;
	bool valid;
};

class CESP {
public:

	void SetDriverHandle(HANDLE hDriver) { m_hDriver = hDriver; }

	void Run(uintptr_t moduleBase);
	std::vector<PlayerData> GetPlayers(uintptr_t moduleBase, int localTeam);

private:

	HANDLE m_hDriver = nullptr;
};

inline CESP ESP;