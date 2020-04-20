#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CWeapon;

class CMonkeyBullet final : public CGameObject
{
public:
	typedef struct tagMonkeyBulletInfo
	{
		tagMonkeyBulletInfo(_v3 _vBirthPos, _v3 _vDir, _float _fSpeed, _double _dLifeTime)
			:vBirthPos(_vBirthPos), vDir(_vDir), fSpeed(_fSpeed), dLifeTime(_dLifeTime)
		{
		}

		_v3						vBirthPos;
		_v3						vDir;
		_float					fSpeed;
		_double					dLifeTime;
	}HUNTERBULLET_INFO;

protected:
	explicit CMonkeyBullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonkeyBullet(const CMonkeyBullet& rhs);
	virtual ~CMonkeyBullet() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	void Update_Collider();
	void Render_Collider();
	void Update_Trails(_double TimeDelta);
	virtual void Enter_Collision();
	virtual void Check_CollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Collider();

public:
	static CMonkeyBullet* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CTransform*			m_pTransformCom = nullptr;
	CCollider*			m_pCollider = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CTrail_VFX*			m_pTrailEffect = nullptr;
	CEffect*			m_pBulletBody = nullptr;
	//CWeapon*			m_pWeapon = nullptr; //≈ı√¥π∞∑Œ πŸ≤ﬂΩ√¥Ÿ

	_tchar				m_pEffect_Tag0[MAX_STR] = { 0, };
	_tchar				m_pEffect_Tag1[MAX_STR] = { 0, };
	_tchar				m_pEffect_Tag2[MAX_STR] = { 0, };
	_tchar				m_pEffect_Tag3[MAX_STR] = { 0, };
	_tchar				m_pEffect_Tag4[MAX_STR] = { 0, };
	_tchar				m_pEffect_Tag5[MAX_STR] = { 0, };

	_v3					m_vDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;

	_bool				m_bDead = false;
	_bool				m_bEffect = true;

};

END


