#include "EnemySpawner.h"
#include "../Core/Config.h"
#include "DxLib.h"
#include <algorithm>

void EnemySpawner::Reset()
{
	m_timer = 0.0f;
	m_interval = 0.45f;
	m_fallSpeed = 260.0f;
}

bool EnemySpawner::UpdateAndShouldSpawn(float dt, float timeSec)
{
	m_fallSpeed = 260.0f + timeSec * 8.0f;

	const float target = 0.45f - timeSec * 0.01f;
	m_interval = std::max(0.18f, target);

	m_timer += dt;
	if (m_timer >= m_interval)
	{
		m_timer -= m_interval;
		return true;
	}
	return false;
}

EnemyType EnemySpawner::RollType() const
{
	const int r = GetRand(99);
	if (r < 55) return EnemyType::Small;
	if (r < 85) return EnemyType::Mid;
	return EnemyType::Large;
}

float EnemySpawner::RollSpawnX(float enemyW) const
{
	const int maxX = SCREEN_W - (int)enemyW;
	return (float)GetRand(maxX);
}