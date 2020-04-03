#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CPoisonBullet final : public CGameObject
{
protected:
	explicit CPoisonBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPoisonBullet(const CPoisonBullet& rhs);
	virtual ~CPoisonBullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_tchar				m_pEffect_Tag0[256] = { 0, };
	_tchar				m_pEffect_Tag1[256] = { 0, };
	_tchar				m_pEffect_Tag2[256] = { 0, };
	_tchar				m_pEffect_Tag3[256] = { 0, };
	_tchar				m_pEffect_Tag4[256] = { 0, };

	_tchar				m_pEffect_Tag5[256] = { 0, };
	_tchar				m_pEffect_Tag6[256] = { 0, };
	_tchar				m_pEffect_Tag7[256] = { 0, };

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;

	_float				m_fEffectCreateOffset		= 0.f;
	_float				m_fEffectCreateOffset_Check = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지


private:
	HRESULT Update_Collider();

	HRESULT Draw_Collider();

private:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CPoisonBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END