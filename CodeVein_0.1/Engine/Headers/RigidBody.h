#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CRigidBody : public CComponent
{
private:
	explicit	CRigidBody(_Device _pGraphicDev);
	explicit	CRigidBody(const CRigidBody& rhs);
	virtual		~CRigidBody();

public:
	virtual void LateUpdate();

public:
	void Set_UseGravity(bool _UseGravity);
	void Set_IsGround(bool _Ground);
	void Set_IsAir(bool _Air);
	void Set_IsHit(bool _Hit);
	void Set_IsAttack(bool _Attack);
	void Set_IsFall(bool _Fall);
	void Set_IsJump(bool _Jump);

public:
	void Set_fPower(float _Power);

public:
	void Set_Speed(D3DXVECTOR3 _Speed = { 1.f, 1.f, 1.f });
	void Set_Accel(D3DXVECTOR3 _Accel = { 0.f, 0.f, 0.f });
	void Set_MaxAccel(D3DXVECTOR3 _MaxAccel = { 0.1f, 0.1f, 0.1f });
	void Set_MaxSpeed(D3DXVECTOR3 _MaxSpeed = { 1.f, 1.f, 1.f });

public:
	D3DXVECTOR3 Get_Accel() { return m_tRigid.vAccel; }
	D3DXVECTOR3 Get_MaxAccel() { return m_tRigid.vMaxAccel; }
	D3DXVECTOR3 Get_Speed() { return m_tRigid.vSpeed; }
	D3DXVECTOR3 Get_MaxSpeed() { return m_tRigid.vMaxSpeed; }
	bool Get_IsJump() { return m_tRigid.bIsJump; }
	bool Get_IsGround() { return m_tRigid.bIsGround; }
	bool Get_IsFall() { return m_tRigid.bIsFall; }
	bool Get_IsAir() { return m_tRigid.bIsAir; }
	bool Get_IsHit() { return m_tRigid.bIsHit; }
	
public:
	float Set_Jump(D3DXVECTOR3 _TransForm, float _Time); 
	float Set_Fall(D3DXVECTOR3 _TransForm, float _Time);


public:
	static CRigidBody* Create(_Device _pGraphicDev);

public:
	virtual void			Free();
	virtual CComponent*		Clone_Component(void* _pArg);

private:
	RIGID m_tRigid;

private:
	virtual HRESULT Ready_Component();
};

END