#include "Ball.h"
#include "../Core/Config.h"
#include "Collision.h"

#include "../Input/Input.h"
#include "Paddle.h"

#include "../Render/RenderUtil.h"

#include "DxLib.h"
#include <cmath>

Ball::Ball()
{
	m_radius = 8.0f;
	m_speed = 520.0f;
	m_vel = Vec2(0.0f, -1.0f);
	m_launched = false;
	m_pos = Vec2(SCREEN_W * 0.5f, SCREEN_H * 0.5f);

	ClearTrail_();
	PushTrail_();
}

void Ball::ClearTrail_()
{
	m_trailCount = 0;
	m_trailHead = 0;
	for (int i = 0; i < TRAIL_MAX; i++)
		m_trail[i] = m_pos;
}

void Ball::PushTrail_()
{
	m_trail[m_trailHead] = m_pos;
	m_trailHead = (m_trailHead + 1) % TRAIL_MAX;
	if (m_trailCount < TRAIL_MAX)
		m_trailCount++;
}

void Ball::ResetOnPaddle(const Paddle& paddle)
{
	m_launched = false;
	m_vel = Vec2(0.0f, -1.0f);
	FollowPaddle_(paddle);

	ClearTrail_();
	PushTrail_();
}

bool Ball::Update(float dt, const Input& input, const Paddle& paddle)
{
	if (!m_launched)
	{
		FollowPaddle_(paddle);
		PushTrail_();

		if (input.Triggered(Action::Decide))
			Launch_();

		return false;
	}

	m_pos.x += m_vel.x * m_speed * dt;
	m_pos.y += m_vel.y * m_speed * dt;

	ReflectOnWalls_();
	ReflectOnPaddle_(paddle);

	PushTrail_();

	if (GetAabb().t >= (float)SCREEN_H)
		return true;

	return false;
}

void Ball::Draw(QualityLevel q, int ox, int oy) const
{
	// --- 残像（トレイル） ---
	// 高品質のときだけ少しリッチに（Blendを使うのでLowでは弱め）
	const int trailN = (q == QualityLevel::High) ? m_trailCount : (m_trailCount / 2);
	if (trailN > 1)
	{
		// 古いものほど薄く
		for (int i = 0; i < trailN; i++)
		{
			const int idx = (m_trailHead - 1 - i + TRAIL_MAX) % TRAIL_MAX;
			const Vec2 p = m_trail[idx];

			int alpha = 20 + (trailN - 1 - i) * 12; // 末尾ほど濃く
			if (alpha > 140) alpha = 140;

			SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
			DrawCircle((int)p.x + ox, (int)p.y + oy, (int)m_radius, GetColor(220, 220, 255), TRUE);
		}
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}

	// --- 本体 ---
	const int x = (int)m_pos.x + ox;
	const int y = (int)m_pos.y + oy;
	const int r = (int)m_radius;

	// 簡易Bloom：AABBにGlow箱を当てる（DxLibプリミティブ縛り）
	const Aabb a = GetAabb();
	RenderUtil::DrawGlowBox((int)a.l + ox, (int)a.t + oy, (int)a.r + ox, (int)a.b + oy, GetColor(240, 240, 255), q);

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

bool Ball::ResolveVsAabb(const Aabb& target)
{
	const Aabb b = GetAabb();
	if (!IntersectAabb(target, b))
		return false;

	const float pushL = b.r - target.l;
	const float pushR = target.r - b.l;
	const float pushT = b.b - target.t;
	const float pushB = target.b - b.t;

	const float minX = (pushL < pushR) ? pushL : pushR;
	const float minY = (pushT < pushB) ? pushT : pushB;

	if (minX < minY)
	{
		if (pushL < pushR) m_pos.x -= minX;
		else m_pos.x += minX;
		m_vel.x = -m_vel.x;
	}
	else
	{
		if (pushT < pushB) m_pos.y -= minY;
		else m_pos.y += minY;
		m_vel.y = -m_vel.y;
	}

	if (Abs_(m_vel.x) < 0.05f) m_vel.x = (m_vel.x >= 0.0f) ? 0.05f : -0.05f;
	if (Abs_(m_vel.y) < 0.05f) m_vel.y = (m_vel.y >= 0.0f) ? 0.05f : -0.05f;

	m_vel = Normalize_(m_vel);
	return true;
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

float Ball::Abs_(float v)
{
	return (v < 0.0f) ? -v : v;
}