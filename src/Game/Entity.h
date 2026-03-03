#pragma once
#include "Vec2.h"
#include "Aabb.h"

// Day7最終：Entityは「共通データ（pos/size/alive）」だけ持つ軽量基底
class Entity
{
public:
	bool IsAlive() const { return m_alive; }
	void Kill() { m_alive = false; }

	Aabb GetAabb() const
	{
		// 左上基準（posは左上）
		return Aabb{ m_pos.x, m_pos.y, m_pos.x + m_size.x, m_pos.y + m_size.y };
	}

	const Vec2& Pos() const { return m_pos; }
	const Vec2& Size() const { return m_size; }

protected:
	Vec2 m_pos{};
	Vec2 m_size{ 32.0f, 32.0f };
	bool m_alive = true;
};