#include "GameScene.h"
#include "SceneManager.h"
#include "SceneType.h"

#include "../Input/Input.h"
#include "../Audio/Audio.h"
#include "../Core/Config.h"

#include "../Game/Player.h"
#include "../Game/EnemySpawner.h"
#include "../Game/Collision.h"

#include "../Render/Camera2D.h"

#include "DxLib.h"
#include <algorithm>
#include <cstdio>

static Vec2 SizeFromType(EnemyType t)
{
	switch (t)
	{
	case EnemyType::Small: return Vec2(24.0f, 24.0f);
	case EnemyType::Mid:   return Vec2(40.0f, 40.0f);
	case EnemyType::Large: return Vec2(64.0f, 32.0f);
	default:               return Vec2(24.0f, 24.0f);
	}
}

GameScene::GameScene(SceneManager* mgr)
	: m_mgr(mgr)
{
	m_player = new Player();
	m_spawner = new EnemySpawner();
	m_camera = new Camera2D();

	m_spawner->Reset();
	m_camera->Reset();
}

GameScene::~GameScene()
{
	delete m_player;
	delete m_spawner;
	delete m_camera;
}

void GameScene::Enter()
{
	m_time = 0.0f;
	m_enemies.clear();

	delete m_player;
	m_player = new Player();

	m_spawner->Reset();
	m_camera->Reset();

	m_isGameOver = false;
	m_goToResultTimer = 0.0f;

	m_hitStopTimer = 0.0f;
	m_flashTimer = 0.0f;
	m_flashDuration = 0.0f;
}

void GameScene::TriggerGameOver_()
{
	if (m_isGameOver) return;
	m_isGameOver = true;

	m_mgr->Context().lastScore = m_time;

	Audio::Instance().PlaySe("hit");

	// 演出セット
	m_hitStopTimer = 0.12f;
	m_camera->AddShake(12.0f, 0.25f);

	m_flashDuration = 0.12f;
	m_flashTimer = m_flashDuration;

	m_goToResultTimer = 0.60f;
}

void GameScene::Update(float dt, const Input& input)
{
	if (input.Triggered(Action::ToggleFps))
		m_mgr->Context().showFps = !m_mgr->Context().showFps;

	if (input.Triggered(Action::ForceLow))
		m_mgr->Context().quality = QualityLevel::Low;

	// 演出タイマーは常に進める
	m_camera->Update(dt);
	if (m_flashTimer > 0.0f)
	{
		m_flashTimer -= dt;
		if (m_flashTimer < 0.0f) m_flashTimer = 0.0f;
	}

	// デバッグ導線（BackでResult）
	if (!m_isGameOver && input.Triggered(Action::Back))
	{
		Audio::Instance().PlaySe("back");
		m_mgr->Context().lastScore = m_time;
		m_mgr->RequestChange(SceneType::Result);
		return;
	}

	// GameOver中：遷移待ち
	if (m_isGameOver)
	{
		m_goToResultTimer -= dt;
		if (m_goToResultTimer <= 0.0f)
			m_mgr->RequestChange(SceneType::Result);
		return;
	}

	// HitStop中：ゲーム更新停止
	if (m_hitStopTimer > 0.0f)
	{
		m_hitStopTimer -= dt;
		if (m_hitStopTimer < 0.0f) m_hitStopTimer = 0.0f;
		return;
	}

	// ===== 通常更新 =====
	m_time += dt;
	m_player->Update(dt, input);

	if (m_spawner->UpdateAndShouldSpawn(dt, m_time))
	{
		const EnemyType type = m_spawner->RollType();
		const Vec2 size = SizeFromType(type);

		const float x = m_spawner->RollSpawnX(size.x);
		const float y = -size.y;
		const float fall = m_spawner->CurrentFallSpeed();

		m_enemies.emplace_back(Vec2(x, y), size, fall, type);
	}

	for (auto& e : m_enemies) e.Update(dt);

	m_enemies.erase(
		std::remove_if(m_enemies.begin(), m_enemies.end(),
			[](const Enemy& e) { return !e.IsAlive(); }),
		m_enemies.end()
	);

	// 衝突
	const Aabb p = m_player->GetAabb();
	for (const auto& e : m_enemies)
	{
		if (IntersectAabb(p, e.GetAabb()))
		{
			TriggerGameOver_();
			break;
		}
	}
}

void GameScene::DrawFlashOverlay_() const
{
	if (m_flashTimer <= 0.0f || m_flashDuration <= 0.0f) return;

	const float t = m_flashTimer / m_flashDuration; // 1 -> 0
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

	// Background
	DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(6, 8, 14), TRUE);

	if (q == QualityLevel::High)
	{
		for (int x = 0; x < SCREEN_W; x += 80)
			DrawLine(x + ox / 4, 0, x + ox / 4, SCREEN_H, GetColor(10, 14, 24));
	}
	else
	{
		for (int x = 0; x < SCREEN_W; x += 160)
			DrawLine(x, 0, x, SCREEN_H, GetColor(10, 14, 24));
	}

	const int white = GetColor(240, 240, 240);
	const int gray = GetColor(180, 180, 180);

	DrawString(40, 40, "GAME", white);

	char buf[128];
	std::snprintf(buf, sizeof(buf), "SCORE: %.2f", m_time);
	DrawString(40, 80, buf, white);

	// Draw objects (quality affects glow)
	m_player->Draw(q, ox, oy);
	for (const auto& e : m_enemies) e.Draw(q, ox, oy);

	if (m_isGameOver)
		DrawString(SCREEN_W / 2 - 80 + ox, SCREEN_H / 2 - 20 + oy, "GAME OVER", GetColor(255, 220, 220));

	DrawFlashOverlay_();
}