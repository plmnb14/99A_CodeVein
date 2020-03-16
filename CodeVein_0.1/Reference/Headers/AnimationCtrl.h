#pragma once

#include "Base.h"

BEGIN(Engine)

class CAnimationCtrl final : public CBase
{
public:
	enum ANIMATION_SET { ANI_DEFAULT = 9999999 };
private:
	explicit CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER pAnimationCtrl);
	explicit CAnimationCtrl(const CAnimationCtrl& rhs);
	virtual ~CAnimationCtrl() = default;
public:
	HRESULT Ready_Animation();
	HRESULT SetUp_Animation(_uint iIndex); // 특정 애니메이션을 재생할 수 있도록 준비시킨다.
	HRESULT Play_Animation(_double TimeDelta); // 애니메이션을 재생한다.
	_bool Finish_Animation();
private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	LPD3DXANIMATIONCONTROLLER	m_pAnimationCtrl = nullptr;
private:
	_uint		m_iOldAniIndex = ANI_DEFAULT;
	_uint		m_iCurrentTrack = 0;
	_uint		m_iNewTrack = 1;
	_double		m_TimeDeltaAcc = 0.0;
	_double		m_Period = 0.0;
public:
	static CAnimationCtrl* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER	pAnimationCtrl);
	virtual CAnimationCtrl* Clone_AniCtrl();
	virtual void Free();
};

END