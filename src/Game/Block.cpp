#include "Block.h"
#include "../Core/Quality.h"

#include "DxLib.h"

Block::Block(float l, float t, float r, float b)
{
	m_aabb.l = l;
	m_aabb.t = t;
	m_aabb.r = r;
	m_aabb.b = b;
	m_alive = true;
}

void Block::Draw(QualityLevel q, int ox, int oy) const
{
	if (!m_alive) return;

	const int x0 = (int)m_aabb.l + ox;
	const int y0 = (int)m_aabb.t + oy;
	const int x1 = (int)m_aabb.r + ox;
	const int y1 = (int)m_aabb.b + oy;

	// Quality‚Å­‚µ‚¾‚¯Œ©‚½–Ú‚ğ•Ï‚¦‚éid‚­‚µ‚È‚¢j
	const int fill = (q == QualityLevel::High)
		? GetColor(80, 170, 240)
		: GetColor(60, 120, 170);

	DrawBox(x0, y0, x1, y1, fill, TRUE);
	DrawBox(x0, y0, x1, y1, GetColor(20, 20, 30), FALSE);
}