#pragma once
#include "Entity.h"
#include "../Core/Quality.h"

class Input;

class Player : public Entity
{
public:
	Player();

	void Update(float dt, const Input& input);
	void Draw(QualityLevel q, int offsetX = 0, int offsetY = 0) const;

private:
	float m_speed = 420.0f; // px/sec
};