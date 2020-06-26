#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"
#include "ParticleMgr.h"

class CMonster_Bullet abstract : public CGameObject
{
public:
	enum MONSTER_BULLET_TYPE
	{
		BULLET_NORMAL, //±‚∫ª «Õµ¢¿Ã ªˆªÛ
		BULLET_FIRE, //∫“¿Ã∆Â∆Æ
		BULLET_ICE, //æÛ¿Ω¿Ã∆Â∆Æ
		BULLET_ELECTRON //¿¸±‚¿Ã∆Â∆Æ
	};

	struct MONSTER_BULLET_STATUS
	{
		MONSTER_BULLET_STATUS(MONSTER_BULLET_TYPE _eType, _v3 _vCreatePos, _v3 _vDir, _float _fSpeed, _double _dLifeTime)
			: eBulletType(_eType), vCreatePos(_vCreatePos), vDir(_vDir), fSpeed(_fSpeed), dLifeTime(_dLifeTime)
		{}
		_v3			vCreatePos = _v3(0.f, 0.f, 0.f);
		_v3			vDir = _v3(0.f, 0.f, 0.f);
		_float		fSpeed = 0.f;
		_double		dLifeTime = 0;
		MONSTER_BULLET_TYPE	eBulletType;
	};

protected:
	explicit CMonster_Bullet(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonster_Bullet(const CMonster_Bullet& rhs);
	virtual ~CMonster_Bullet() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader) PURE;

	virtual void Update_Trails();
	virtual void Update_Collider();
	virtual void Render_Collider();
	virtual void Check_CollisionEvent();
	virtual void Check_CollisionHit(list<CGameObject*> plistGameObject);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();

protected:
	CTrail_VFX*				m_pTrailEffect = nullptr;
	CEffect*				m_pEffect = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

	MONSTER_BULLET_TYPE		m_eBulletType = MONSTER_BULLET_TYPE::BULLET_NORMAL;

	_v3						m_vDir = V3_NULL;
	_float					m_fSpeed = 0.f;
	_float					m_fEffectOffset = 0.f;

	_double					m_dCurTime = 0;
	_double					m_dLifeTime = 0;

	_bool					m_bDead = false;
	_bool					m_bEffect = true;
	_bool					m_bPlayerFriendly = false;

};

