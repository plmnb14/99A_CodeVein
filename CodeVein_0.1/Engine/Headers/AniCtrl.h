#pragma once

#include "Base.h"

BEGIN(Engine)

class CAniCtrl final : public CBase
{
public:
	enum ANIMATION_SET { ANI_DEFAULT = 9999999 };
private:
	explicit CAniCtrl(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER pAnimationCtrl);
	explicit CAniCtrl(const CAniCtrl& rhs);
	virtual ~CAniCtrl() = default;
public:
	HRESULT Ready_Animation();
	HRESULT SetUp_Animation(_uint iIndex , _bool _bOffLerp = false); // 특정 애니메이션을 재생할 수 있도록 준비시킨다.
	HRESULT Play_Animation(_double TimeDelta); // 애니메이션을 재생한다.
	_bool Is_Finish_Animation(_float _fLerpValue);

public:
	D3DXTRACK_DESC Get_TrackInfo();
	_double Get_AnimationFullTime() { return m_Period; };
	LPD3DXANIMATIONCONTROLLER Get_AniCtrl() { return m_pAnimationCtrl; }

public:
	void Reset_OldIdx();

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
	static CAniCtrl* Create(LPDIRECT3DDEVICE9 pGraphic_Device, LPD3DXANIMATIONCONTROLLER	pAnimationCtrl);
	virtual CAniCtrl* Clone_AniCtrl();
	virtual void Free();
};

END