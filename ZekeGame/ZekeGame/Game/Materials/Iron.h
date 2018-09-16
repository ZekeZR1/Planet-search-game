#pragma once
#include "character\CharacterController.h"
class Inventory;

class Iron
{
public:
	Iron();
	~Iron();
	void Update(Player* m_player, Inventory* inventory);
	void Draw();
	void SetPosition(CVector3 pos);
	bool isNear(CVector3 pos);
	bool isGet = false;
private:
	SkinModel* m_skinModel;
	CVector3 m_pos = CVector3::Zero();
	float mf_near = 100.0f;
};
