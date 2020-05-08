#pragma once

#include "Monster.h"

BEGIN(Client)

class CPoisonChaseBullet final : public CMonster
{
protected:
	explicit CPoisonChaseBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPoisonChaseBullet(const CPoisonChaseBullet& rhs);
	virtual ~CPoisonChaseBullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	CEffect*			m_pBulletBody = nullptr;

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_tchar				m_pEffect_Tag0[STR_128] = { 0, };
	_tchar				m_pEffect_Tag1[STR_128] = { 0, };
	_tchar				m_pEffect_Tag2[STR_128] = { 0, };
	_tchar				m_pEffect_Tag3[STR_128] = { 0, };
	_tchar				m_pEffect_Tag4[STR_128] = { 0, };

	_tchar				m_pEffect_Tag5[STR_128] = { 0, };
	_tchar				m_pEffect_Tag6[STR_128] = { 0, };
	_tchar				m_pEffect_Tag7[STR_128] = { 0, };
	_tchar				m_pEffect_Tag8[STR_128] = { 0, };
	_tchar				m_pEffect_Tag9[STR_128] = { 0, };
	_tchar				m_pEffect_Tag10[STR_128] = { 0, };

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
	void Check_PhyCollider();

	virtual void OnCollisionEnter();
	virtual void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Collider();

public:
	static CPoisonChaseBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END