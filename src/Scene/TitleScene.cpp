#include "TitleScene.h"
#include "SceneManager.h"
#include "SceneType.h"

#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Core/Config.h"

#include "DxLib.h"
#include <cstdio>

void TitleScene::Enter()
{
	m_blink = 0.0f;
	m_showPress = true;
}

void TitleScene::Update(float dt, const Input& input)
{
	m_blink += dt;
	if (m_blink >= 0.5f)
	{
		m_blink = 0.0f;
		m_showPress = !m_showPress;
	}

	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		if (m_mgr->Context().quality == QualityLevel::High) {
			m_mgr->Context().quality = QualityLevel::Low;
		}
		else {
			m_mgr->Context().quality = QualityLevel::High;
		}
		

	if (input.Triggered(Action::Decide))
	{
		Audio::Instance().PlaySe("decide");
		m_mgr->RequestChange(SceneType::Game);
		return;
	}
}

void TitleScene::Draw()
{
	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(10, 12, 18), TRUE);

	for (int y = 0; y < SCREEN_H; y += 40)
		DrawLine(0, y, SCREEN_W, y, GetColor(16, 20, 30));

	const int white = GetColor(240, 240, 240);
	const int gray = GetColor(180, 180, 180);

	DrawString(40, 40, "Game A Week - Week1 (Dodge Drop)", gray);
	DrawString(40, 100, "DODGE DROP", white);

	DrawString(40, 170, "Move: Left/Right  Decide: Enter/Space/PadA", gray);
	DrawString(40, 200, "Back: Esc/PadB (Result)  F1: Toggle FPS  F2: Force LOW", gray);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "HI-SCORE: %.2f", m_mgr->Context().hiScore);
	DrawString(40, 250, buf, white);

	if (m_showPress)
		DrawString(40, 340, "PRESS DECIDE TO START", white);
}