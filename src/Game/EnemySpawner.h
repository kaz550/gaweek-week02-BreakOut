#pragma once
#include "Enemy.h"

class EnemySpawner
{
public:
	void Reset();

	bool UpdateAndShouldSpawn(float dt, float timeSec);

	float CurrentFallSpeed() const { return m_fallSpeed; }
	EnemyType RollType() const;
	float RollSpawnX(float enemyW) const;

private:
	float m_timer = 0.0f;
	float m_interval = 0.45f;
	float m_fallSpeed = 260.0f;
};