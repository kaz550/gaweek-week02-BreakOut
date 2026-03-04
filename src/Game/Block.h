#pragma once
#include "Aabb.h"

enum class QualityLevel;

class Block
{
public:
	Block() = default;
	Block(float l, float t, float r, float b, int hp);

	void Draw(QualityLevel q, int ox, int oy) const;
	Aabb GetAabb() const { return m_aabb; }

	bool IsAlive() const { return m_alive; }

	int Hp() const { return m_hp; }
	int MaxHp() const { return m_maxHp; }

	// ñﬂÇËílÅFîjâÛÇ≥ÇÍÇΩÇÁ true
	bool Damage(int amount = 1);

private:
	Aabb m_aabb{};
	bool m_alive = true;
	int  m_hp = 1;
	int  m_maxHp = 1;
};