#include "RigidBody.h"

CRigidBody::CRigidBody(_Device _pGraphicDev)
	:CComponent(_pGraphicDev)
{
	ZeroMemory(&m_tRigid, sizeof(RIGID));
}

CRigidBody::CRigidBody(const CRigidBody& rhs)
	:CComponent(rhs)
{
	memcpy(&m_tRigid, &rhs.m_tRigid, sizeof(RIGID));
}

CRigidBody::~CRigidBody()
{
}

void CRigidBody::LateUpdate()
{
}

void CRigidBody::Set_UseGravity(bool _UseGravity)
{
	m_tRigid.bIsGravity = _UseGravity;
}

void CRigidBody::Set_IsGround(bool _Ground)
{
	m_tRigid.bIsGround = _Ground;
}

void CRigidBody::Set_IsAir(bool _Air)
{
	m_tRigid.bIsAir = _Air;
}

void CRigidBody::Set_IsHit(bool _Hit)
{
	m_tRigid.bIsHit = _Hit;
}

void CRigidBody::Set_IsAttack(bool _Attack)
{
	m_tRigid.bIsAttck = _Attack;
}

void CRigidBody::Set_IsFall(bool _Fall)
{
	m_tRigid.bIsFall = _Fall;
}

void CRigidBody::Set_IsJump(bool _Jump)
{
	m_tRigid.bIsJump = _Jump;
}

void CRigidBody::Set_fPower(float _Power)
{
	m_tRigid.fPower = _Power;
}

void CRigidBody::Set_Speed(D3DXVECTOR3 _Speed)
{
	m_tRigid.vSpeed = _Speed;
}

void CRigidBody::Set_Accel(D3DXVECTOR3 _Accel)
{
	m_tRigid.vAccel = _Accel;
}

void CRigidBody::Set_MaxAccel(D3DXVECTOR3 _MaxAccel)
{
	m_tRigid.vMaxAccel = _MaxAccel;
}

void CRigidBody::Set_MaxSpeed(D3DXVECTOR3 _MaxSpeed)
{
	m_tRigid.vMaxSpeed = _MaxSpeed;
}

float CRigidBody::Set_Jump(D3DXVECTOR3 _TransForm , float _Time)
{
	m_tRigid.vAccel.y -= m_tRigid.vMaxAccel.y * _Time;

	return (m_tRigid.fPower * m_tRigid.vAccel.y + GRAVITY * m_tRigid.vAccel.y * m_tRigid.vAccel.y * 0.5f) * _Time;
}

float CRigidBody::Set_Fall(D3DXVECTOR3 _TransForm, float _Time)
{
	if (!m_tRigid.bIsGravity)
		return 0;

	if (m_tRigid.vAccel.y < m_tRigid.vMaxAccel.y)
		m_tRigid.vAccel.y += m_tRigid.vMaxAccel.y * _Time;

	return ((m_tRigid.fPower * m_tRigid.vAccel.y + GRAVITY * m_tRigid.vAccel.y * m_tRigid.vAccel.y * 0.5f) * _Time) * 0.5f;
}

CRigidBody* CRigidBody::Create(_Device _pGraphicDev)
{
	CRigidBody* pComponent = new CRigidBody(_pGraphicDev);

	if (FAILED(pComponent->Ready_Component()))
	{
		Safe_Release(pComponent);
		return nullptr;
	}

	return pComponent;
}

void CRigidBody::Free()
{
	CComponent::Free();
}

HRESULT CRigidBody::Ready_Component()
{
	return S_OK;
}

CComponent * CRigidBody::Clone_Component(void* _pArg)
{
	return new CRigidBody(*this);
}
