#pragma once
#include "IScene.h"

class SceneManager;
class Camera2D;

class Paddle;
class Ball;

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

	int m_lives = 3;
	float m_time = 0.0f;
};