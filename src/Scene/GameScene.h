#pragma once
#include "IScene.h"

class SceneManager;
class Camera2D;

class Paddle;

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

	float m_time = 0.0f;
};