#pragma once
#include "Vec2.h"
#include "Aabb.h"

class Input;
class Paddle;

enum class QualityLevel;

class Ball
{
public:
	Ball();

	void ResetOnPaddle(const Paddle& paddle);

	// 戻り値：落下したら true
	bool Update(float dt, const Input& input, const Paddle& paddle);
	void Draw(QualityLevel q, int ox, int oy) const;

	bool IsLaunched() const { return m_launched; }
	Aabb GetAabb() const;

	// Day4: AABB相手に「面判定＋押し戻し＋反射」する
	// 戻り値：衝突して解決したら true
	bool ResolveVsAabb(const Aabb& target);

private:
	Vec2 m_pos;     // 中心
	Vec2 m_vel;     // 単位方向（正規化済み）
	float m_speed = 0.0f;
	float m_radius = 0.0f;
	bool m_launched = false;

	void Launch_();
	void FollowPaddle_(const Paddle& paddle);
	void ReflectOnWalls_();
	void ReflectOnPaddle_(const Paddle& paddle);

	static float Clamp_(float v, float lo, float hi);
	static float Length_(const Vec2& v);
	static Vec2 Normalize_(const Vec2& v);
	static float Abs_(float v);
};