#pragma once
#include "graphics\SpringCamera.h"
class SpringCamera;
class Player;

class GameCamera
{
public:
	GameCamera();
	~GameCamera();
	void Update(Player* player);
	Player* m_player;	
	CVector3 m_toCameraPos;	
	SpringCamera m_springCamera;
};

