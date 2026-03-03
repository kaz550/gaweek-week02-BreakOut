#pragma once
#include <array>

enum class Action
{
	Left,
	Right,
	Decide,
	Back,
	Quit,

	ToggleFps, // F1
	ForceLow,  // F2

	COUNT
};

class Input
{
public:
	void Update();

	bool Pressed(Action a) const;
	bool Triggered(Action a) const;
	bool Released(Action a) const;

	float GetMoveX() const { return m_moveX; }

private:
	std::array<int, (int)Action::COUNT> m_prev{};
	std::array<int, (int)Action::COUNT> m_curr{};
	float m_moveX = 0.0f;

	int GetActionDown_(Action a) const;

	int GetPadState_() const;
	static int IsKeyDown_(int key);
	static int IsBitOn_(int state, int bit);
};