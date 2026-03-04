#include "Paddle.h"

#include "../Input/Input.h"
#include "../Core/Config.h"
#include "../Core/Quality.h"

#include "DxLib.h"
#include <algorithm>

Paddle::Paddle()
{
    Reset();
}

void Paddle::Reset()
{
    m_size = Vec2(140.0f, 18.0f);
    m_speed = 760.0f;
    m_pos = Vec2((SCREEN_W - m_size.x) * 0.5f, SCREEN_H - 80.0f);
}

void Paddle::Update(float dt, const Input& input)
{
    float dir = 0.0f;
    if (input.Pressed(Action::Left))  dir -= 1.0f;
    if (input.Pressed(Action::Right)) dir += 1.0f;

    m_pos.x += dir * m_speed * dt;

    // âÊñ äOÇ…èoÇ»Ç¢
    const float minX = 20.0f;
    const float maxX = (float)SCREEN_W - 20.0f - m_size.x;
    m_pos.x = std::clamp(m_pos.x, minX, maxX);
}

void Paddle::Draw(QualityLevel, int ox, int oy) const
{
    const int x0 = (int)m_pos.x + ox;
    const int y0 = (int)m_pos.y + oy;
    const int x1 = (int)(m_pos.x + m_size.x) + ox;
    const int y1 = (int)(m_pos.y + m_size.y) + oy;

    DrawBox(x0, y0, x1, y1, GetColor(240, 240, 240), TRUE);
    DrawBox(x0, y0, x1, y1, GetColor(40, 40, 40), FALSE);
}

Aabb Paddle::GetAabb() const
{
    Aabb a;
    a.l = m_pos.x;
    a.t = m_pos.y;
    a.r = m_pos.x + m_size.x;
    a.b = m_pos.y + m_size.y;
    return a;
}