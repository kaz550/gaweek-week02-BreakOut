#include "GameScene.h"
#include "SceneManager.h"
#include "SceneType.h"

#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Core/Config.h"

#include "../Render/Camera2D.h"
#include "../Game/Paddle.h"

#include "DxLib.h"
#include <cstdio>

GameScene::GameScene(SceneManager* mgr)
	: m_mgr(mgr)
{
	m_camera = new Camera2D();
	m_paddle = new Paddle();

	m_camera->Reset();
}

GameScene::~GameScene()
{
	delete m_paddle;
	delete m_camera;
}

void GameScene::Enter()
{
	m_time = 0.0f;
	m_camera->Reset();
	m_paddle->Reset();
}

void GameScene::Update(float dt, const Input& input)
{
	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		m_mgr->Context().quality = QualityLevel::Low;

	// Day1はデバッグ導線として Back で Resultへ飛べるようにしておく
	if (input.Triggered(Action::Back))
	{
		Audio::Instance().PlaySe("back");
		m_mgr->Context().lastScore = 0.0f;
		m_mgr->RequestChange(SceneType::Result);
		return;
	}

	m_time += dt;
	m_camera->Update(dt);
	m_paddle->Update(dt, input);
}

void GameScene::Draw()
{
	const int ox = m_camera->OffsetX();
	const int oy = m_camera->OffsetY();
	const QualityLevel q = m_mgr->Context().quality;

	// Background
	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(6, 8, 14), TRUE);

	// Grid（軽い見た目の目印）
	for (int x = 0; x < SCREEN_W; x += (q == QualityLevel::High ? 80 : 160))
		DrawLine(x, 0, x, SCREEN_H, GetColor(10, 14, 24));
	for (int y = 0; y < SCREEN_H; y += (q == QualityLevel::High ? 40 : 80))
		DrawLine(0, y, SCREEN_W, y, GetColor(10, 14, 24));

	const int white = GetColor(240, 240, 240);
	const int gray = GetColor(180, 180, 180);

	DrawString(40, 40, "GAME (Day1 Prototype)", white);
	DrawString(40, 80, "Paddle only. Ball/Blocks will be added on Day2+", gray);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "TIME: %.2f", m_time);
	DrawString(40, 110, buf, gray);

	m_paddle->Draw(q, ox, oy);
}