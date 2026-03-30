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

#include "../Game/Collision.h" // ★ 正しい呼び出し

#include "DxLib.h"
#include <cstdio>

static int ClampI_(int v, int lo, int hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

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

	m_hitStopTimer = 0.0f;
	m_flashTimer = 0.0f;
	m_flashDuration = 0.0f;

	// stageIndex の安全化
	m_mgr->Context().stageCount = 3;
	m_mgr->Context().stageIndex = ClampI_(m_mgr->Context().stageIndex, 0, m_mgr->Context().stageCount - 1);
	m_mgr->Context().resultKind = ResultKind::None;

	m_camera->Reset();
	m_paddle->Reset();
	m_ball->ResetOnPaddle(*m_paddle);

	BuildStage_();
}

void GameScene::GoResult_(ResultKind kind)
{
	m_mgr->Context().resultKind = kind;
	m_mgr->Context().lastScore = (float)m_score;
	m_mgr->RequestChange(SceneType::Result);
}

void GameScene::TriggerBlockHitFx_(bool destroyed)
{
	Audio::Instance().PlaySe("hit");

	m_hitStopTimer = destroyed ? 0.06f : 0.02f;
	m_camera->AddShake(destroyed ? 10.0f : 5.0f, destroyed ? 0.18f : 0.10f);

	m_flashDuration = destroyed ? 0.10f : 0.06f;
	m_flashTimer = m_flashDuration;
}

void GameScene::Update(float dt, const Input& input)
{
	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		m_mgr->Context().quality = QualityLevel::Low;

	// 演出タイマーは常に進める（Week1思想）
	m_camera->Update(dt);
	if (m_flashTimer > 0.0f)
	{
		m_flashTimer -= dt;
		if (m_flashTimer < 0.0f) m_flashTimer = 0.0f;
	}

	// デバッグ導線：BackでResult（GameOver扱いにしておく）
	if (input.Triggered(Action::Back))
	{
		Audio::Instance().PlaySe("back");
		m_mgr->Context().stageIndex = 0;
		GoResult_(ResultKind::GameOver);
		return;
	}

	// HitStop中：ゲーム更新は止める
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= dt;
		if (m_hitStopTimer < 0.0f) m_hitStopTimer = 0.0f;
		return;
	}

	m_time += dt;
	m_paddle->Update(dt, input);

	const bool fell = m_ball->Update(dt, input, *m_paddle);
	if (fell)
	{
		Audio::Instance().PlaySe("hit");
		m_lives--;
		if (m_lives <= 0)
		{
			// GameOver：ステージは最初に戻す
			m_mgr->Context().stageIndex = 0;
			GoResult_(ResultKind::GameOver);
			return;
		}

		m_paddle->Reset();
		m_ball->ResetOnPaddle(*m_paddle);
	}

	CheckBallVsBlocks_();

	// クリア判定
	if (AliveBlocks_() == 0)
	{
		Audio::Instance().PlaySe("decide");

		const int next = m_mgr->Context().stageIndex + 1;
		if (next >= m_mgr->Context().stageCount)
		{
			// ALL CLEAR：次は最初から
			m_mgr->Context().stageIndex = 0;
			GoResult_(ResultKind::AllClear);
		}
		else
		{
			// 次ステージへ
			m_mgr->Context().stageIndex = next;
			GoResult_(ResultKind::StageClear);
		}
		return;
	}
}

void GameScene::DrawFlashOverlay_() const
{
	if (m_flashTimer <= 0.0f || m_flashDuration <= 0.0f) return;

	float t = m_flashTimer / m_flashDuration; // 1 -> 0
	int alpha = (int)(180 * t);
	if (alpha < 0) alpha = 0;
	if (alpha > 255) alpha = 255;

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(255, 255, 255), TRUE);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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

	DrawString(40, 40, "GAME (Day7)", white);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "STAGE: %d / %d", m_mgr->Context().stageIndex + 1, m_mgr->Context().stageCount);
	DrawString(40, 80, buf, white);

	std::snprintf(buf, sizeof(buf), "LIVES: %d", m_lives);
	DrawString(40, 110, buf, white);
	std::snprintf(buf, sizeof(buf), "SCORE: %d", m_score);
	DrawString(40, 140, buf, white);

	DrawString(40, 170, "Decide: Launch", gray);
	DrawString(40, 200, "Back: Result (Debug)", gray);

	for (const auto& b : m_blocks)
		b.Draw(q, ox, oy);

	m_paddle->Draw(q, ox, oy);
	m_ball->Draw(q, ox, oy);

	DrawFlashOverlay_();
}

void GameScene::BuildStage_()
{
	m_blocks.clear();

	// 画面に収まる寸法（800幅想定）
	const int cols = 10;
	const float pad = 5.0f;
	const float bw = 70.0f;
	const float bh = 26.0f;

	const float totalW = cols * bw + (cols - 1) * pad;
	const float startX = ((float)SCREEN_W - totalW) * 0.5f;
	const float startY = 120.0f;

	const int stage = m_mgr->Context().stageIndex;

	if (stage == 0)
	{
		// Stage1：基本（上ほど硬い）
		const int rows = 5;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				const float x0 = startX + c * (bw + pad);
				const float y0 = startY + r * (bh + pad);
				const float x1 = x0 + bw;
				const float y1 = y0 + bh;

				int hp = 1;
				if (r == 0) hp = 3;
				else if (r <= 2) hp = 2;
				else hp = 1;

				m_blocks.emplace_back(x0, y0, x1, y1, hp);
			}
		}
	}
	else if (stage == 1)
	{
		// Stage2：隙間あり（チェッカー）
		const int rows = 6;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				if (((r + c) % 2) == 1) continue; // 隙間

				const float x0 = startX + c * (bw + pad);
				const float y0 = startY + r * (bh + pad);
				const float x1 = x0 + bw;
				const float y1 = y0 + bh;

				int hp = (r <= 1) ? 3 : (r <= 3 ? 2 : 1);
				m_blocks.emplace_back(x0, y0, x1, y1, hp);
			}
		}
	}
	else
	{
		// Stage3：ピラミッド
		const int rows = 7;
		for (int r = 0; r < rows; r++)
		{
			for (int c = 0; c < cols; c++)
			{
				const int left = r;
				const int right = cols - 1 - r;
				if (c < left || c > right) continue;

				const float x0 = startX + c * (bw + pad);
				const float y0 = startY + r * (bh + pad);
				const float x1 = x0 + bw;
				const float y1 = y0 + bh;

				int hp = 1;
				if (r <= 1) hp = 3;
				else if (r <= 3) hp = 2;
				else hp = 1;

				m_blocks.emplace_back(x0, y0, x1, y1, hp);
			}
		}
	}
}

void GameScene::CheckBallVsBlocks_()
{
	for (auto& blk : m_blocks)
	{
		if (!blk.IsAlive()) continue;

		const Aabb a = blk.GetAabb();
		if (!m_ball->ResolveVsAabb(a))
			continue;

		const bool destroyed = blk.Damage(1);
		TriggerBlockHitFx_(destroyed);

		// スコア：ヒットでも少し、破壊なら多め
		m_score += destroyed ? 100 : 10;
		break; // 1フレーム1個
	}
}

int GameScene::AliveBlocks_() const
{
	int n = 0;
	for (const auto& b : m_blocks)
		if (b.IsAlive()) n++;
	return n;
}