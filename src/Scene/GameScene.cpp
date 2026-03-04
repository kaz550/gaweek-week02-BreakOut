#include "GameScene.h"
#include "SceneManager.h"
#include "SceneType.h"

#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Core/Config.h"

#include "../Render/Camera2D.h"
#include "../Game/Paddle.h"
#include "../Game/Ball.h"

#include "DxLib.h"
#include <cstdio>

GameScene::GameScene(SceneManager* mgr)
	: m_mgr(mgr)
{
	m_camera = new Camera2D();
	m_paddle = new Paddle();
	m_ball = new Ball();

	m_camera->Reset();
}

GameScene::~GameScene()
{
	delete m_ball;
	delete m_paddle;
	delete m_camera;
}

void GameScene::Enter()
{
	m_time = 0.0f;
	m_lives = 3;

	m_camera->Reset();
	m_paddle->Reset();
	m_ball->ResetOnPaddle(*m_paddle);
}

void GameScene::Update(float dt, const Input& input)
{
	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		m_mgr->Context().quality = QualityLevel::Low;

	// Day2もデバッグ導線として Back で Resultへ飛べるようにしておく
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

	const bool fell = m_ball->Update(dt, input, *m_paddle);
	if (fell)
	{
		Audio::Instance().PlaySe("hit");
		m_lives--;
		if (m_lives <= 0)
		{
			// Day2は暫定：Resultへ
			m_mgr->Context().lastScore = 0.0f;
			m_mgr->RequestChange(SceneType::Result);
			return;
		}

		// リスポーン
		m_paddle->Reset();
		m_ball->ResetOnPaddle(*m_paddle);
	}
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

	DrawString(40, 40, "GAME (Day2)", white);
	DrawString(40, 80, "Decide: Launch", gray);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "LIVES: %d", m_lives);
	DrawString(40, 110, buf, white);

	m_paddle->Draw(q, ox, oy);
	m_ball->Draw(q, ox, oy);
}