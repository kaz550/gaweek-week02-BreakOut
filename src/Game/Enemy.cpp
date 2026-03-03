#include "Enemy.h"
#include "../Core/Config.h"
#include "../Render/RenderUtil.h"
#include "DxLib.h"

Enemy::Enemy(const Vec2& pos, const Vec2& size, float fallSpeed, EnemyType type)
{
	m_pos = pos;
	m_size = size;
	m_fallSpeed = fallSpeed;
	m_type = type;
}

void Enemy::Update(float dt)
{
	m_pos.y += m_fallSpeed * dt;
	if (m_pos.y > SCREEN_H + 80.0f) m_alive = false;
}

void Enemy::Draw(QualityLevel q, int offsetX, int offsetY) const
{
	const int x1 = (int)(m_pos.x + offsetX);
	const int y1 = (int)(m_pos.y + offsetY);
	const int x2 = (int)(m_pos.x + m_size.x + offsetX);
	const int y2 = (int)(m_pos.y + m_size.y + offsetY);

	int fill = 0;
	int edge = 0;

	switch (m_type)
	{
	case EnemyType::Small:
		fill = GetColor(255, 90, 120);
		edge = GetColor(255, 220, 230);
		break;
	case EnemyType::Mid:
		fill = GetColor(255, 160, 80);
		edge = GetColor(255, 235, 210);
		break;
	case EnemyType::Large:
		fill = GetColor(170, 90, 255);
		edge = GetColor(230, 220, 255);
		break;
	}

	RenderUtil::DrawGlowBox(x1, y1, x2, y2, fill, q);

	DrawBox(x1, y1, x2, y2, fill, TRUE);
	DrawBox(x1, y1, x2, y2, edge, FALSE);
	DrawBox(x1 + 2, y1 + 2, x2 - 2, y2 - 2, GetColor(20, 20, 20), FALSE);
}