#pragma once
#include "Entity.h"
#include "../Core/Quality.h"

enum class EnemyType
{
	Small,
	Mid,
	Large
};

class Enemy : public Entity
{
public:
	Enemy(const Vec2& pos, const Vec2& size, float fallSpeed, EnemyType type);

	void Update(float dt);
	void Draw(QualityLevel q, int offsetX = 0, int offsetY = 0) const;

	EnemyType Type() const { return m_type; }

private:
	float m_fallSpeed = 260.0f;
	EnemyType m_type = EnemyType::Small;
};