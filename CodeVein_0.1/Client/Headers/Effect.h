#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CEffect final : public CGameObject
{
public:
	typedef struct tagEffectInfo
	{
		TCHAR	szName[256];
		TCHAR	szGradientName[256];
		_float	fMaxFrame;
		
		_float	fAnimSpeed;
		_float  fLifeTime;
		
		_int	iMaxCount; // For Particle
		
		_float  fMaxAlpha;
		_float	fAlphaSpeed;

		_bool	bOnlyYRot;

		_bool	bLinearMove;
		_bool	bEaseInMove;
		_bool	bEaseOutMove;
		_float	fMinSpeed;
		_float	fMaxSpeed;
		_float	fMoveSpeed;

		_v3		vStartPos;
		_float	fRansStartPosRange_Min[AXIS_END];
		_float	fRansStartPosRange_Max[AXIS_END];
		_bool	bRandStartPos;

		_bool	bDistortion;

		_bool	bDirMove;
		_v3		vMoveDirection;

		_bool	bScaleMove;
		_v3		vStartScale;
		_v3		vMoveScale;
		_float	fMoveScaleSpeed;

		_bool	bRandomMove;

		_bool	bRotMove;
		_v3		vRotDirection;
		_float	fRotSpeed;

		_bool	bColorMove;
		_v4		vStartColor;
		_v4		vEndColor;
		_float	fColorSpeed;
	}EFFECT_INFO;

	typedef struct tagEffDesc
	{
		CTransform* pTargetTrans;
		_v3			vWorldPos;
	}EFFECT_DESC;
protected:
	explicit CEffect(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;
public:
	virtual HRESULT Ready_GameObject_Prototype(); 
	virtual HRESULT Ready_GameObject(void* pArg); 
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pGradientTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	EFFECT_INFO*			m_pInfo = nullptr;
	EFFECT_DESC*			m_pDesc = nullptr;

	_float					m_fFrame	= 0.f;
	_float					m_fAlpha	= 1.f;
	_float					m_fLifeTime = 0.f;
	_v3						m_vLerpScale	= { 1.f, 1.f, 1.f };
	_v3						m_vDir			= { 1.f, 1.f, 1.f };
	_v3						m_vFollowPos	= { 1.f, 1.f, 1.f };
	_v4						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_int					m_iPass = 0;
	_bool					m_bClone = false;

private:
	void Check_Frame(_double TimeDelta);
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);

	void Setup_Billboard();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END