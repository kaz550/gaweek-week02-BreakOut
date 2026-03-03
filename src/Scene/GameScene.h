#pragma once
#include "IScene.h"
#include <vector>

class SceneManager;

namespace RenderUtil {}
class Camera2D;

class Player;
class EnemySpawner;

#include "../Game/Enemy.h" // vector<Enemy> ‚ðŽg‚¤‚½‚ß

class GameScene : public IScene
{
public:
	explicit GameScene(SceneManager* mgr);
	~GameScene();

	void Enter() override;
	void Update(float dt, const class Input& input) override;
	void Draw() override;

private:
	void TriggerGameOver_();
	void DrawFlashOverlay_() const;

private:
	SceneManager* m_mgr = nullptr;

	Player* m_player = nullptr;
	EnemySpawner* m_spawner = nullptr;
	std::vector<Enemy> m_enemies;

	Camera2D* m_camera = nullptr;

	float m_time = 0.0f;

	bool  m_isGameOver = false;
	float m_goToResultTimer = 0.0f;

	float m_hitStopTimer = 0.0f;
	float m_flashTimer = 0.0f;
	float m_flashDuration = 0.0f;
};