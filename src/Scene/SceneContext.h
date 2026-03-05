#pragma once
#include "../Core/Quality.h"

enum class ResultKind
{
	None,
	GameOver,
	StageClear,
	AllClear,
};

struct SceneContext
{
	float lastScore = 0.0f;
	float hiScore = 0.0f;

	bool showFps = true;
	float fps = 60.0f;
	QualityLevel quality = QualityLevel::High;

	// Week2: ステージ制と結果
	ResultKind resultKind = ResultKind::None;
	int stageIndex = 0;      // 次に遊ぶステージ（ResultのDecideでGameへ）
	int stageCount = 3;      // 固定（Day6）
};