#include "Ball.h"
#include "../Core/Config.h"
#include "Collision.h"

#include "../Input/Input.h"
#include "Paddle.h"

#include "DxLib.h"
#include <cmath>

Ball::Ball()
{
	m_radius = 8.0f;
	m_speed = 520.0f;
	m_vel = Vec2(0.0f, -1.0f);
	m_launched = false;
	m_pos = Vec2(SCREEN_W * 0.5f, SCREEN_H * 0.5f);
}

void Ball::ResetOnPaddle(const Paddle& paddle)
{
	m_launched = false;
	m_vel = Vec2(0.0f, -1.0f);
	FollowPaddle_(paddle);
}

bool Ball::Update(float dt, const Input& input, const Paddle& paddle)
{
	if (!m_launched)
	{
		FollowPaddle_(paddle);

		if (input.Triggered(Action::Decide))
			Launch_();

		return false;
	}

	m_pos.x += m_vel.x * m_speed * dt;
	m_pos.y += m_vel.y * m_speed * dt;

	ReflectOnWalls_();
	ReflectOnPaddle_(paddle);

	if (GetAabb().t >= (float)SCREEN_H)
		return true;

	return false;
}

void Ball::Draw(QualityLevel, int ox, int oy) const
{
	const int x = (int)m_pos.x + ox;
	const int y = (int)m_pos.y + oy;
	const int r = (int)m_radius;

	DrawCircle(x, y, r, GetColor(250, 250, 250), TRUE);
	DrawCircle(x, y, r, GetColor(40, 40, 40), FALSE);
}

Aabb Ball::GetAabb() const
{
	Aabb a;
	a.l = m_pos.x - m_radius;
	a.t = m_pos.y - m_radius;
	a.r = m_pos.x + m_radius;
	a.b = m_pos.y + m_radius;
	return a;
}

void Ball::BounceY()
{
	m_vel.y = -m_vel.y;
	// ”½ŽËŒã‚ÉY‚ª0•t‹ß‚É‚È‚éŽ–ŒÌ‚ð”ð‚¯‚éiŽb’èj
	if (m_vel.y > -0.2f && m_vel.y < 0.2f)
		m_vel.y = (m_vel.y >= 0.0f) ? 0.2f : -0.2f;
	m_vel = Normalize_(m_vel);
}

void Ball::Launch_()
{
	m_launched = true;
	m_vel = Vec2(0.0f, -1.0f);
}

void Ball::FollowPaddle_(const Paddle& paddle)
{
	const Aabb p = paddle.GetAabb();
	m_pos.x = (p.l + p.r) * 0.5f;
	m_pos.y = p.t - m_radius - 1.0f;
}

void Ball::ReflectOnWalls_()
{
	if (m_pos.x - m_radius < 0.0f)
	{
		m_pos.x = m_radius;
		m_vel.x = -m_vel.x;
	}
	else if (m_pos.x + m_radius > (float)SCREEN_W)
	{
		m_pos.x = (float)SCREEN_W - m_radius;
		m_vel.x = -m_vel.x;
	}

	if (m_pos.y - m_radius < 0.0f)
	{
		m_pos.y = m_radius;
		m_vel.y = -m_vel.y;
	}
}

void Ball::ReflectOnPaddle_(const Paddle& paddle)
{
	if (m_vel.y <= 0.0f) return;

	const Aabb p = paddle.GetAabb();
	const Aabb b = GetAabb();
	if (!IntersectAabb(p, b)) return;

	m_pos.y = p.t - m_radius - 0.5f;

	const float cx = (p.l + p.r) * 0.5f;
	const float half = (p.r - p.l) * 0.5f;
	float nx = 0.0f;
	if (half > 1e-3f)
		nx = (m_pos.x - cx) / half;

	nx = Clamp_(nx, -1.0f, 1.0f);

	Vec2 dir(nx, -1.0f);
	dir = Normalize_(dir);
	m_vel = dir;
}

float Ball::Clamp_(float v, float lo, float hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

float Ball::Length_(const Vec2& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 Ball::Normalize_(const Vec2& v)
{
	const float len = Length_(v);
	if (len <= 1e-6f) return Vec2(0.0f, -1.0f);
	return Vec2(v.x / len, v.y / len);
}