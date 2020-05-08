#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CSwordGenjiBullet final : public CGameObject
{
protected:
	explicit CSwordGenjiBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordGenjiBullet(const CSwordGenjiBullet& rhs);
	virtual ~CSwordGenjiBullet() = default;

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
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pBulletBody = nullptr;

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_tchar				m_pEffect_Tag0[STR_128] = { 0, };
	_tchar				m_pEffect_Tag1[STR_128] = { 0, };
	_tchar				m_pEffect_Tag2[STR_128] = { 0, };
	_tchar				m_pEffect_Tag3[STR_128] = { 0, };
	_tchar				m_pEffect_Tag4[STR_128] = { 0, };
	_tchar				m_pEffect_Tag5[STR_128] = { 0, };

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;

	_float				m_fEffectCreateOffset = 0.f;
	_float				m_fEffectCreateOffset_Check = 0.f;
private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

private:
	HRESULT Update_Collider();
	HRESULT Draw_Collider();
	void Update_Trails(_double TimeDelta);

private:
	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CSwordGenjiBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END