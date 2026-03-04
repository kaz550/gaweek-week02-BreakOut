#pragma once
#include "Vec2.h"
#include "Aabb.h"

class Input;

enum class QualityLevel;

class Paddle
{
public:
    Paddle();

    void Reset();
    void Update(float dt, const Input& input);
    void Draw(QualityLevel q, int ox, int oy) const;

    Aabb GetAabb() const;

private:
    Vec2 m_pos;   // ç∂è„
    Vec2 m_size;
    float m_speed = 0.0f;
};