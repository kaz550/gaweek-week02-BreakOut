#pragma once
#include "IScene.h"
#include "SceneContext.h" // Åö à»å„Ç±ÇÃÉãÅ[ÉãÇ≈ìùàÍ
#include <vector>

class SceneManager;
class Camera2D;

class Paddle;
class Ball;
class Block;

class GameScene : public IScene
{
public:
	explicit GameScene(SceneManager* mgr);
	~GameScene();

	void Enter() override;
	void Update(float dt, const class Input& input) override;
	void Draw() override;

private:
	SceneManager* m_mgr = nullptr;

	Camera2D* m_camera = nullptr;
	Paddle* m_paddle = nullptr;
	Ball* m_ball = nullptr;

	std::vector<Block> m_blocks;

	int m_lives = 3;
	int m_score = 0;
	float m_time = 0.0f;

	float m_hitStopTimer = 0.0f;
	float m_flashTimer = 0.0f;
	float m_flashDuration = 0.0f;

	void BuildStage_();
	void CheckBallVsBlocks_();
	int AliveBlocks_() const;

	void TriggerBlockHitFx_(bool destroyed);
	void DrawFlashOverlay_() const;

	void GoResult_(ResultKind kind);
};