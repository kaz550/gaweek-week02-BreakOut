#pragma once
#include "Aabb.h"

enum class QualityLevel;

class Block
{
public:
	Block() = default;
	Block(float l, float t, float r, float b);

	void Draw(QualityLevel q, int ox, int oy) const;
	Aabb GetAabb() const { return m_aabb; }

	bool IsAlive() const { return m_alive; }
	void Kill() { m_alive = false; }

private:
	Aabb m_aabb{};
	bool m_alive = true;
};