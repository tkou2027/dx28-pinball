#pragma once

class GameGlobal
{
public:
	int GetScore() const { return m_score; }
	void SetScore(int score) { m_score = score; }
private:
	int m_score{ 0 };
};

extern GameGlobal g_game_global;