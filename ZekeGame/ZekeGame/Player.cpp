#include "stdafx.h"
#include "Player.h"
#include "GameScene.h"

Player::Player()
{
	//m_model.Init(L"Assets/modelData/unityChan.cmo");
	m_model.Init(L"Assets/modelData/Player.cmo", enFbxUpAxisY);
	m_position.y += 500.0f;
	m_charaCon = new CharacterController;
	m_charaCon->Init(30.0f, 50.0f, m_position);

	ms_battery = new Sprite;
	m_batteryPos.x += 300.0f;
	m_batteryPos.y += 300.0f;
	ms_battery->Init(L"sprite/BatteryGauge.dds", 300.0f, 50.0f);

	ms_bFrame.Init(L"sprite/BatteryFrame.dds", 400.0f, 100.0f);
	m_bFramePos = m_batteryPos;
	m_bFramePos.x -= 50.0f;
	m_bFramePos.y -= 27.0f;
	ms_bFrame.Update(m_bFramePos, CQuaternion::Identity(), CVector3::One(), { 0.0f,0.0f });
}


Player::~Player()
{
	delete m_charaCon;
	delete ms_battery;
}

void Player::Update() {
	CanOpenMenu();
	Move();
	Turn();
	ChangeState();
	CharaconUpdate();
	Gauge();
}

void Player::Move() {
	if (m_enPState == PSTATE_WALK) {
		if (g_pad[0].IsPress(enButtonRB2)) {
			//m_moveSpeedParam = 1300.0f;
			m_moveSpeedParam = 7000.0f;
		}else
			m_moveSpeedParam = 750.0f;
	}
	else {
		m_moveSpeedParam = 600.0f;
	}
	if (isLowBattery)
		m_moveSpeedParam = 500.0f;
	if (isRiding) {
		m_moveSpeedParam = 2500.0f;
		m_moveSpeed.y = 0;
	}
	CVector3 cameraForward = camera3d->GetTarget() - camera3d->GetPosition();
	cameraForward.y = 0.0f;
	cameraForward.Normalize();
	m_moveSpeed.x = cameraForward.x * m_moveSpeedParam * g_pad[0].GetLStickYF();
	m_moveSpeed.z = cameraForward.z * m_moveSpeedParam * g_pad[0].GetLStickYF();
	CVector3 cameraRight;
	cameraRight.Cross({ 0.0f, 1.0f, 0.0f }, cameraForward);
	cameraRight.Normalize();
	m_moveSpeed += cameraRight * m_moveSpeedParam * g_pad[0].GetLStickXF();
	if (g_pad[0].IsTrigger(enButtonA)) {
		if(!isRiding)
			m_moveSpeed.y += 700.0f;
	}
	if (!isRiding) {
		m_moveSpeed.y -= 50.0f;
	}
	CQuaternion qRot;
	qRot.SetRotationDeg(CVector3::AxisX(), 180.0f);
	qRot.Multiply(m_rotation, qRot);
	m_position = m_charaCon->Execute(1.0f / 60.0f, m_moveSpeed);
	m_model.UpdateWorldMatrix(m_position, m_rotation, CVector3::One());
}

void Player::Turn() {
	if (fabsf(m_moveSpeed.x) < 0.001f
		&& fabsf(m_moveSpeed.z) < 0.001f) {
		return;
	}
	float angle = atan2(m_moveSpeed.x, m_moveSpeed.z);
	m_rotation.SetRotation(CVector3::AxisY(), angle);
}

void Player::Draw() {
	m_model.Draw(camera3d->GetViewMatrix(),camera3d->GetProjectionMatrix());
}

void Player::DrawSprite() {
	ms_bFrame.Draw();
	ms_battery->Draw();
}

void Player::ChangeState() {
	if (m_enPState != PSTATE_MAKEGROUND)
		return;
	if (g_pad[0].IsTrigger(enButtonRB1)) {
		if(mi_state != 2)
			mi_state++;
	}
	if (g_pad[0].IsTrigger(enButtonLB1)) {
		if(mi_state != 0)
			mi_state--;
	}
	switch (mi_state) {
	case 0:
		ActState = State_Leveling;
		break;
	case 1:
		ActState = State_Mining;
		break;
	case 2:
		ActState = State_Fill;
		break;
	}
}

bool Player::isNear(CVector3 thatpos, float dist) {
	CVector3 t = m_position - thatpos;
	float diff = t.Length();
	if (diff < dist) {
		return true;
	}
	else {
		return false;
	}
}

bool Player::CanOpenMenu() {
	if (m_enPState != PSTATE_WALK)
		return false;
	if (m_nMenu > 0) {
		//m_player->isOpenMenuNow = true;
		char message[256];
		sprintf_s(message, "NOOOOOOO NOW %d\n",m_nMenu);
		OutputDebugStringA(message);
		return false;
	}
	else {
		//m_player->isOpenMenuNow = false;
		char message[256];
		sprintf_s(message, "YEEEEEEEEEES %d\n", m_nMenu);
		OutputDebugStringA(message);
		return true;
	}
}

void Player::OpenMenu() {
	m_nMenu++;
	isOpenMenuNow = true;
}

void Player::CloseMenu() {
	m_nMenu--;
	isOpenMenuNow = false;
}

void Player::CharaconUpdate() {
	if (isRiding && !isCharaConRide) {
		delete m_charaCon;
		m_charaCon = new CharacterController;
		m_charaCon->Init(500.0f, 60.0f, m_position);
		isCharaConRide = true;
	}
	else {
		if (isCharaConRide) {
			delete m_charaCon;
			m_charaCon = new CharacterController;
			m_charaCon->Init(30.0f, 50.0f, m_position);
			isCharaConRide = false;
		}
	}
}

void Player::Gauge() {
	if (m_batteryScale.x >= 1.0f) {
		isMaxBattery = true;
	}
	else {
		isMaxBattery = false;
	}
	if (m_batteryScale.x < 0.0f) {
		isLowBattery = true;
	}
	else {
		isLowBattery = false;
	}
	if(!isMaxBattery)
		m_batteryScale.x += 0.0002f;

	if (isRiding)
		return;
	if (m_moveSpeed.x != 0.0f || m_moveSpeed.z != 0.0f) {
		if (!isLowBattery)
			m_batteryScale.x -= 0.001f;
	}
	ms_battery->Update(m_batteryPos, CQuaternion::Identity(), m_batteryScale, { 0.0f,0.0f });
}

void Player::UseBattery() {
	m_batteryScale.x -= 0.002f;
}

void Player::ChargeBattery() {
	if(m_batteryScale.x >= 1.0f)
	m_batteryScale.x += 0.01f;
}