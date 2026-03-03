#pragma once
#include "../Core/Quality.h"

struct SceneContext
{
	float lastScore = 0.0f;
	float hiScore = 0.0f;

	bool showFps = true;
	float fps = 60.0f;
	QualityLevel quality = QualityLevel::High;
};