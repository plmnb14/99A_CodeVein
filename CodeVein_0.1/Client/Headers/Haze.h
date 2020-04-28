#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "Trail_VFX.h"
#include "Effect.h"

BEGIN(Client)

class CHaze final : public CGameObject
{
public:
	typedef struct tagHazeInfo
	{
		tagHazeInfo(_float _fValue, _v3 _vPos, _float _fDelay)
			: fHazeValue(_fValue), vCreatePos(_vPos), fDelay(_fDelay)
		{}
		_float fHazeValue;
		_v3 vCreatePos;
		_float fDelay;
	}HAZE_INFO;

protected:
	explicit CHaze(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CHaze(const CHaze& rhs);
	virtual ~CHaze() = default;

public:
	_float Get_HazeValue() { return m_fHazeValue; }

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

	CTransform*			m_pTargetTrans = nullptr;

	_float				m_fHazeValue = 0.f;

	_v3					m_vCurveRotDir = _v3(0.f, 0.f, 0.f);
	_float				m_fSpeed = 0.f;
	_float				m_fCurveAccel = 0.f;
	_float				m_fCurvePower = 0.f;

	_float				m_fDelay = 0.f;

	_double				m_dCurTime = 0;
	_double				m_dLifeTime = 0;
	_bool				m_bDead = false;

	_bool				m_bEffect = true;
	_float				m_fEffectOffset = 0.f;

private:
	void Check_Move(_double TimeDelta);
	void Check_Delay(_double TimeDelta);
	void Update_Trails(_double TimeDelta);

private:

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CHaze* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END