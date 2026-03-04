#include "GameScene.h"
#include "SceneManager.h"
#include "SceneType.h"

#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Core/Config.h"

#include "../Render/Camera2D.h"
#include "../Game/Paddle.h"
#include "../Game/Ball.h"
#include "../Game/Block.h"

#include "../Game/Collision.h"

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
	m_score = 0;

	m_camera->Reset();
	m_paddle->Reset();
	m_ball->ResetOnPaddle(*m_paddle);

	BuildStage_();
}

void GameScene::Update(float dt, const Input& input)
{
	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		m_mgr->Context().quality = QualityLevel::Low;

	if (input.Triggered(Action::Back))
	{
		Audio::Instance().PlaySe("back");
		m_mgr->Context().lastScore = (float)m_score;
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
			m_mgr->Context().lastScore = (float)m_score;
			m_mgr->RequestChange(SceneType::Result);
			return;
		}

		m_paddle->Reset();
		m_ball->ResetOnPaddle(*m_paddle);
	}

	CheckBallVsBlocks_();

	// Day3：暫定クリア
	if (AliveBlocks_() == 0)
	{
		Audio::Instance().PlaySe("decide");
		m_mgr->Context().lastScore = (float)m_score;
		m_mgr->RequestChange(SceneType::Result);
		return;
	}
}

void GameScene::Draw()
{
	const int ox = m_camera->OffsetX();
	const int oy = m_camera->OffsetY();
	const QualityLevel q = m_mgr->Context().quality;

	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(6, 8, 14), TRUE);

	for (int x = 0; x < SCREEN_W; x += (q == QualityLevel::High ? 80 : 160))
		DrawLine(x, 0, x, SCREEN_H, GetColor(10, 14, 24));
	for (int y = 0; y < SCREEN_H; y += (q == QualityLevel::High ? 40 : 80))
		DrawLine(0, y, SCREEN_W, y, GetColor(10, 14, 24));

	const int white = GetColor(240, 240, 240);
	const int gray = GetColor(180, 180, 180);

	DrawString(40, 40, "GAME (Day3)", white);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "LIVES: %d", m_lives);
	DrawString(40, 80, buf, white);
	std::snprintf(buf, sizeof(buf), "SCORE: %d", m_score);
	DrawString(40, 110, buf, white);

	// Blocks
	for (const auto& b : m_blocks)
		b.Draw(q, ox, oy);

	m_paddle->Draw(q, ox, oy);
	m_ball->Draw(q, ox, oy);
}

void GameScene::BuildStage_()
{
	m_blocks.clear();

	// 1ステージ固定（Day6でステージ制）
	const int rows = 5;
	const int cols = 10;

	const float pad = 6.0f;
	const float bw = 100.0f;
	const float bh = 28.0f;

	const float startX = 80.0f;
	const float startY = 120.0f;

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			const float x0 = startX + c * (bw + pad);
			const float y0 = startY + r * (bh + pad);
			const float x1 = x0 + bw;
			const float y1 = y0 + bh;

			m_blocks.emplace_back(x0, y0, x1, y1);
		}
	}
}

void GameScene::CheckBallVsBlocks_()
{
	const Aabb ball = m_ball->GetAabb();

	for (auto& blk : m_blocks)
	{
		if (!blk.IsAlive()) continue;

		const Aabb a = blk.GetAabb();
		if (!IntersectAabb(a, ball)) continue;

		blk.Kill();
		m_score += 100; // Day3は仮

		Audio::Instance().PlaySe("hit");

		// Day3は暫定：Y反転だけ
		m_ball->BounceY();
		break; // 1フレーム1個だけ壊す
	}
}

int GameScene::AliveBlocks_() const
{
	int n = 0;
	for (const auto& b : m_blocks)
		if (b.IsAlive()) n++;
	return n;
}