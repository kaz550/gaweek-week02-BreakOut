#include "Player.h"
#include "../Input/Input.h"
#include "../Core/Config.h"
#include "../Render/RenderUtil.h"
#include "DxLib.h"

Player::Player()
{
	m_size = Vec2(64.0f, 24.0f);
	m_pos.x = (SCREEN_W - m_size.x) * 0.5f;
	m_pos.y = SCREEN_H - 80.0f;
}

void Player::Update(float dt, const Input& input)
{
	const float moveX = input.GetMoveX();
	m_pos.x += moveX * m_speed * dt;

	if (m_pos.x < 0.0f) m_pos.x = 0.0f;
	if (m_pos.x > (float)SCREEN_W - m_size.x) m_pos.x = (float)SCREEN_W - m_size.x;
}

void Player::Draw(QualityLevel q, int offsetX, int offsetY) const
{
	const int x1 = (int)(m_pos.x + offsetX);
	const int y1 = (int)(m_pos.y + offsetY);
	const int x2 = (int)(m_pos.x + m_size.x + offsetX);
	const int y2 = (int)(m_pos.y + m_size.y + offsetY);

	const int body = GetColor(40, 200, 255);

	RenderUtil::DrawGlowBox(x1, y1, x2, y2, body, q);

	DrawBox(x1, y1, x2, y2, body, TRUE);
	DrawBox(x1, y1, x2, y2, GetColor(240, 240, 240), FALSE);
	DrawBox(x1 + 2, y1 + 2, x2 - 2, y2 - 2, GetColor(0, 80, 110), FALSE);
	DrawLine((x1 + x2) / 2, y1, (x1 + x2) / 2, y2, GetColor(220, 220, 220));
}