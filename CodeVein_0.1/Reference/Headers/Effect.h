#pragma once

#include "RenderObject.h"
#include "Management.h"

BEGIN(Engine)

class ENGINE_DLL CEffect : public CGameObject
{
public:

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
	EFFECT_INFO* Get_Info() { return m_pInfo; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
protected:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CTexture*				m_pGradientTextureCom = nullptr;
	CTexture*				m_pColorTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

protected:
	EFFECT_INFO*			m_pInfo = nullptr;
	EFFECT_DESC*			m_pDesc = nullptr;

	_float					m_fFrame = 0.f;
	_float					m_fAlpha = 1.f;
	_float					m_fLifeTime = 0.f;
	_float					m_fMoveSpeed = 0.f;
	_float					m_fLinearMoveSpeed = 0.f;
	_float					m_fLinearMovePercent = 0.f;
	_float					m_fRotSpeed = 0.f;
	_float					m_fAlphaSpeed = 0.f;
	_float					m_fCreateDelay = 0.f;
	_v3						m_vLerpPos = { 1.f, 1.f, 1.f };
	_v3						m_vLerpScale = { 1.f, 1.f, 1.f };
	_v3						m_vDir = { 1.f, 1.f, 1.f };
	_v3						m_vRot = { 0.f, 0.f, 0.f };
	_v3						m_vFollowPos = { 1.f, 1.f, 1.f };
	_v4						m_vColor = { 1.f, 1.f, 1.f, 1.f };

	_int					m_iPass = 0;
	_bool					m_bFadeOutStart = false;
	_bool					m_bClone = false;

protected:
	void Check_Frame(_double TimeDelta);
	void Check_Move(_double TimeDelta);
	void Check_LifeTime(_double TimeDelta);
	void Check_Alpha(_double TimeDelta);
	void Check_Color(_double TimeDelta);
	void Check_CreateDelay(_double TimeDelta);

	void Setup_Billboard();

protected:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	void Change_EffectTexture(const _tchar* _Name);
	void Change_GradientTexture(const _tchar* _Name);
	void Change_ColorTexture(const _tchar* _Name);

public:
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	static CEffect* Create(LPDIRECT3DDEVICE9 pGraphic_Device, EFFECT_INFO* pInfo);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END