#include "Block.h"
#include "../Core/Quality.h"

#include "DxLib.h"

static int ClampI(int v, int lo, int hi)
{
	if (v < lo) return lo;
	if (v > hi) return hi;
	return v;
}

Block::Block(float l, float t, float r, float b, int hp)
{
	m_aabb.l = l;
	m_aabb.t = t;
	m_aabb.r = r;
	m_aabb.b = b;

	m_maxHp = ClampI(hp, 1, 3);
	m_hp = m_maxHp;
	m_alive = true;
}

bool Block::Damage(int amount)
{
	if (!m_alive) return false;

	m_hp -= amount;
	if (m_hp <= 0)
	{
		m_hp = 0;
		m_alive = false;
		return true;
	}
	return false;
}

void Block::Draw(QualityLevel q, int ox, int oy) const
{
	if (!m_alive) return;

	const int x0 = (int)m_aabb.l + ox;
	const int y0 = (int)m_aabb.t + oy;
	const int x1 = (int)m_aabb.r + ox;
	const int y1 = (int)m_aabb.b + oy;

	// HPで色を変える（1=弱い/3=硬い）
	// DxLibには GetColorR/G/B のような分解関数が無いので、
	// Quality差分は「別の色を用意する」方式にします。
	int fillHigh = 0;
	int fillLow = 0;

	if (m_maxHp >= 3)
	{
		if (m_hp == 3) { fillHigh = GetColor(80, 170, 240); fillLow = GetColor(50, 110, 160); }
		else if (m_hp == 2) { fillHigh = GetColor(120, 200, 120); fillLow = GetColor(80, 140, 80); }
		else { fillHigh = GetColor(240, 190, 80); fillLow = GetColor(170, 130, 50); }
	}
	else if (m_maxHp == 2)
	{
		if (m_hp == 2) { fillHigh = GetColor(120, 200, 120); fillLow = GetColor(80, 140, 80); }
		else { fillHigh = GetColor(240, 190, 80); fillLow = GetColor(170, 130, 50); }
	}
	else
	{
		fillHigh = GetColor(240, 190, 80);
		fillLow = GetColor(170, 130, 50);
	}

	const int fill = (q == QualityLevel::High) ? fillHigh : fillLow;

	DrawBox(x0, y0, x1, y1, fill, TRUE);
	DrawBox(x0, y0, x1, y1, GetColor(20, 20, 30), FALSE);
}